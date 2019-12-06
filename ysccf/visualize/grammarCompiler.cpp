#include "grammarCompiler.h"

#include "../base/errorReport.h"
#include "../utils/invokeful.hpp"


grammarCompiler::grammarCompiler()
{
    initGrammarCompiler();
}

grammarCompiler::~grammarCompiler()
{
    if( pGrammarParser != nullptr )
        delete pGrammarParser;
}

lexer::tokenStream grammarCompiler::tokenize(stringType str)
{
    return compilerLexer().tokenize(str);
}

pSyntaxTree grammarCompiler::parse(const lexer::tokenStream& stream)
{
    return pGrammarParser->parse(stream);
}

const grammar &grammarCompiler::construct(pSyntaxTree pRoot)
{
    clear();

    chainSyntaxVisit(
        pRoot,
        [this](pcSyntaxTree pNode,nodeType node){
        if( node == _statement ){
            parseStatement(pNode);
        }
        return false;
    });

    return customGrammarContainer;
}


void grammarCompiler::analyzeParser(pcSyntaxTree tree)
{
    customGrammarContainer.clear();
    customLexer.clear();
    customTokenTable.clear();
    customSymbolTable.clear();

    chainSyntaxVisit(
        tree,
        [this](pcSyntaxTree pNode,nodeType node){
        if( node == _statement ){
            parseStatement(pNode);
        }
        return false;
    });
}

void grammarCompiler::parseStatement(pcSyntaxTree tree)
{
    chainSyntaxVisit(
        tree,
        [this](pcSyntaxTree pNode,nodeType node){
        if( node == _tokenDef ){
            parseToken(pNode);
        }
        else if( node == _rule ){
            parseRule(pNode);
        }
        return false;
    });
}

void grammarCompiler::parseToken(pcSyntaxTree tree)
{
    stringType tokenName,tokenStr;
    auto visitors = overloaded(
        [this,&tokenName,&tokenStr](const lexer::tokenUnit* ptoken){
            if( ptoken->id == _identifier ){
                tokenName = ptoken->info.raw;
            }
            else if ( ptoken->id == _stringConst ){
                tokenStr = processStringConst(ptoken->info.raw);
            }
        },
        []([[maybe_unused]]producerType producer){

        }
    );
    chainSyntaxVisit(
        tree,
        [&visitors](pcSyntaxTree pNode,nodeType node){
        if( node == nodeNotExist ){
            std::visit(visitors,pNode->data());
        }
        return false;
    });
    auto ret = declareNewToken(tokenName,tokenStr);
}

void grammarCompiler::parseRule(pcSyntaxTree tree)
{
    stringType producerName;
    nodeType producerID;
    nodeType vNodeID;
    vtType strType;
    declareState decRet;
    lexer::tokenID tokenID;
    const lexer::tokenUnit* pToken = nullptr;

    auto visitors = overloaded(
        [&vNodeID,&pToken](const lexer::tokenUnit* ptoken){
            vNodeID = nodeNotExist;
            pToken = ptoken;
        },
        [&vNodeID](producerType producer){
            vNodeID = producer;
        }
    );

    auto iter = tree->children().rbegin();
    auto ender = tree->children().rend();

    do{
        std::visit(visitors,(*iter)->data());
        if(vNodeID != _producer)
            break;
        std::tie(producerName,strType) = parseV((*iter));
        if( strType == vttFailed )
            break;

        std::tie(producerID,decRet) = declareNewSymbol(producerName);
        if(decRet == dsUndefined)
            break;

        ++iter;

        std::visit(visitors,(*iter)->data());
        if(vNodeID != nodeNotExist ||
           pToken->id != _deducer)
            break;

        ++iter;

        std::visit(visitors,(*iter)->data());
        if(vNodeID != _produced)
            break;
        parseProduced(producerID,(*iter));

        ++iter;

        std::visit(visitors,(*iter)->data());
        if(vNodeID != _orProduced)
            break;
        parseOrProduced(producerID,(*iter));

        ++iter;
        /*
        chainSyntaxVisit(
            (*iter),
            [this,producerID](pcSyntaxTree pNode,nodeType node){
            if( node == _produced )
                parseProduced(producerID,pNode);
            return false;
        });*/

    }
    while(false);
}

void grammarCompiler::parseProduced(nodeType producer, pcSyntaxTree tree)
{
    production prod;
    chainSyntaxVisit(
        tree,
        [this,&prod](pcSyntaxTree pNode,nodeType node){
            if( node == _V ){
                auto [raw,type] = parseV(pNode);
                nodeType nodeCont;
                declareState dsret;
                switch( type ){
                    case vttVn:
                        std::tie(nodeCont,dsret) =
                                declareNewSymbol(raw);
                        if( dsret == dsUndefined )
                            break;
                        prod.appendSymbol(nodeCont);
                        break;
                    case vttKwNull:
                        break;
                    case vttStringConst:
                        raw = replaced(raw,stringType(L"\\\""),stringType(L"\""));
                        std::tie(nodeCont,dsret) =
                                declareNewToken(stringType(L"_s\"")+raw+L"\"",raw,true);
                        if( dsret == dsUndefined )
                            break;
                        prod.appendToken(nodeCont);
                        break;
                    case vttVtIndtifier:
                        std::tie(nodeCont,dsret) =
                                declareNewToken(raw,stringType(L"诶？!"));
                        switch ( dsret ) {
                        case dsRedefined:
                            prod.appendToken(nodeCont);
                            break;
                        default:
                            errorReport(L"grammarCompiler::parseProduced()",
                                        stringType(L"token \"") + raw + L"\" not defined",
                                        40);
                            break;
                        }
                        break;
                    case vttFailed:
                        break;
                }

            }
            return false;
        }
    );
    customGrammarContainer.addProduction(producer,prod);
}

void grammarCompiler::parseOrProduced(nodeType producer, pcSyntaxTree tree)
{
    chainSyntaxVisit(
        tree,
        [producer,this](pcSyntaxTree pNode,nodeType node){
            if( node == _produced ){
               parseProduced(producer,pNode);
            }
            return false;
        }
    );
}

std::pair<nodeType,grammarCompiler::declareState>
grammarCompiler::
    declareNewToken(
        const stringType& name,
        const stringType& str,
        bool escaped)
{
    if ( !name.empty() && !str.empty() ){
        lexer::tokenID id;
        try {
            id = customTokenTable.at(name);
        } catch (std::out_of_range e) {
            id = customLexer.newToken(str,escaped);
            customTokenTable.insert(name,id);
            return std::make_pair(id,dsSuccess);
        }
        return std::make_pair(id,dsRedefined);
    }
    return std::make_pair(nodeNotExist,dsUndefined);
}

std::pair<nodeType, grammarCompiler::declareState> grammarCompiler::declareNewSymbol(const stringType &name)
{
    if ( !name.empty() ){
        nodeType id;

        try {
            id = customSymbolTable.at(name);
        } catch (std::out_of_range e) {
            id = customGrammarContainer.newSymbol();
            customSymbolTable.insert(name,id);
            return std::make_pair(id,dsSuccess);
        }
        return std::make_pair(id,dsRedefined);
    }
    return std::make_pair(nodeNotExist,dsUndefined);
}

std::pair<stringType, grammarCompiler::vtType> grammarCompiler::parseV(pcSyntaxTree tree)
{
    auto ret = std::pair(stringType(),vttFailed);
    if( tree->children().empty() )
        return ret;

    auto pChild = tree->children().back();

    auto visitors = overloaded(
        [](const lexer::tokenUnit* ptoken){
        },
        [this,pChild,&ret](producerType producer){
            if(producer == this->_Vt){
                ret = parseVt(pChild);
            }
            else if(producer == this->_VnExpr){
                ret.second = vttVn;
                ret.first = parseVnExpr(pChild);
            }
        }
    );

    std::visit(visitors,pChild->data());

    return ret;
}

stringType grammarCompiler::parseVnExpr(pcSyntaxTree tree)
{
    return parseIdentifier(tree->children()[1]->children()[0]);
}

std::pair<stringType, grammarCompiler::vtType> grammarCompiler::parseVt(pcSyntaxTree tree)
{
    auto ret = std::pair(stringType(),vttFailed);
    auto visitors = overloaded(
        [this,&ret](const lexer::tokenUnit* ptoken){
            auto& [raw,state] = ret;
            if(ptoken->id == _identifier ){
                raw = ptoken->info.raw;
                state = vttVtIndtifier;
            }
            else if(ptoken->id == _kwNull ){
                raw = ptoken->info.raw;
                state = vttKwNull;
            }
            else if(ptoken->id == _stringConst ){
                raw = processStringConst(ptoken->info.raw);
                state = vttStringConst;
            }
            //ret.first = ptoken->info.raw;
        },
        [](producerType producer){
        }
    );

    std::visit(visitors,tree->children().back()->data());
    return ret;
}

stringType grammarCompiler::parseIdentifier(pcSyntaxTree tree)
{
    stringType retIdent;
    auto visitors = overloaded(
        [this,&retIdent](const lexer::tokenUnit* ptoken){
            if( ptoken->id == _identifier )
                retIdent = ptoken->info.raw;
        },
        [](producerType producer){
        }
    );

    std::visit(visitors,tree->data());
    return retIdent;
}

stringType grammarCompiler::processStringConst(const stringType &sc)
{
    auto ib = sc.find_first_of('\"');
    auto ie = sc.find_last_of('\"');
    return stringType(sc,ib+1,ie-ib-1);
}

void grammarCompiler::initGrammarCompiler()
{
    _kwNull = lexer.newToken(LR"(\s*null\s*)");
    _kwToken = lexer.newToken(LR"(\s*)" + lexer.tokenDefiner + LR"(\s+)");
    _beforeVn = lexer.newToken(LR"(\s*<\s*)");
    _afterVn = lexer.newToken(LR"(\s*>\s*)");
    _stringConst = lexer.newToken(LR"("(\\.|[^"\\])*")");
    _comment = lexer.newToken(LR"(\s*/\*.*\*/\s*)");
    _deducer = lexer.newToken(LR"(\s*::=\s*)");
    _delimiter = lexer.newToken(LR"(\s*;\s*)");
    _or = lexer.newToken(LR"(\s*\|\s*)");
    _space = lexer.newToken(LR"(\s*)");
    _identifier = lexer.newToken(LR"([a-zA-Z_]\w*)");

    tokenTable.insert(L"kwNull",_kwNull);
    tokenTable.insert(L"kwToken",_kwToken);
    tokenTable.insert(L"beforeVn",_beforeVn);
    tokenTable.insert(L"afterVn",_afterVn);
    tokenTable.insert(L"stringConst",_stringConst);
    tokenTable.insert(L"comment",_comment);
    tokenTable.insert(L"deducer",_deducer);
    tokenTable.insert(L"delimiter",_delimiter);
    tokenTable.insert(L"or",_or);
    tokenTable.insert(L"space",_space);
    tokenTable.insert(L"identifier",_identifier);

    grammar syn;
    _grammarDef = syn.newSymbol();
    _statement = syn.newSymbol();
    _tokenDef = syn.newSymbol();
    _rule = syn.newSymbol();
    _producer = syn.newSymbol();
    _produced = syn.newSymbol();
    _orProduced = syn.newSymbol();
    _V = syn.newSymbol();
    _VnExpr = syn.newSymbol();
    _Vn = syn.newSymbol();
    _Vt = syn.newSymbol();
    syn.markStart(_grammarDef);

    symbolTable.insert(L"grammarDef",_grammarDef);
    symbolTable.insert(L"statement",_statement);
    symbolTable.insert(L"tokenDef",_tokenDef);
    symbolTable.insert(L"rule",_rule);
    symbolTable.insert(L"producer",_producer);
    symbolTable.insert(L"produced",_produced);
    symbolTable.insert(L"orProduced",_orProduced);
    symbolTable.insert(L"V",_V);
    symbolTable.insert(L"VnExpr",_VnExpr);
    symbolTable.insert(L"Vn",_Vn);
    symbolTable.insert(L"Vt",_Vt);


R"kk(
%token kwNull "\s*null\s*"
%token kwToken "\s*%token\s+"
%token beforeVn "\s*<\s*"
%token afterVn "\s*>\s*"
%token stringConst "\"(\\.|[^\"\\])*\""
%token comment "\s*/\*.*\*/\s*"
%token deducer "\s*::=\s*"
%token delimiter "\s*;\s*"
%token or "\s*\|\s*"
%token space "\s*"
%token identifier "[a-zA-Z_]\w*"
<grammarDef> ::= <grammarDef> <statement> | null;
<statement> ::= <tokenDef> | <rule> | comment ;
<tokenDef> ::= kwToken identifier space stringConst ;
<rule> ::= <producer> deducer <produced> <orProduced> delimiter ;
<producer> ::= <VnExpr> ;
<produced> ::= <produced> <V> | null;
<orProduced> ::= or <produced> <orProduced> | null;
<V> ::= <VnExpr> | <Vt> ;
<VnExpr> ::= beforeVn <Vn> afterVn ;
<Vn> ::= identifier ;
<Vt> ::= identifier | identifier space | kwNull | stringConst | stringConst space ;
<ExtendedStart> ::= <grammarDef> ;
)kk";

    production prod;
    //0 <_grammarDef> ::= <_grammarDef> <_statement> | null;
    prod.appendSymbol(_grammarDef)
        .appendSymbol(_statement);
    auto launcher = syn.addProduction(_grammarDef,std::move(prod));
    prod.clear();

    syn.addProduction(_grammarDef,std::move(prod));

    syn.markStart(launcher);

    //1 <_statement> ::= <_tokenDef> | <_rule> | _comment ;
    prod.appendSymbol(_tokenDef);
    syn.addProduction(_statement,std::move(prod));
    prod.clear();

    prod.appendSymbol(_rule);
    syn.addProduction(_statement,std::move(prod));
    prod.clear();

    prod.appendToken(_comment);
    syn.addProduction(_statement,std::move(prod));
    prod.clear();

    //2 <_tokenDef> ::= _kwToken _identifier _space _stringConst;
    prod.appendToken(_kwToken)
        .appendToken(_identifier)
        .appendToken(_space)
        .appendToken(_stringConst);
    syn.addProduction(_tokenDef,std::move(prod));
    prod.clear();

    //3 <_rule> ::= <_producer> _deducer <_produced> <_orProduced> _delimiter ;
    prod.appendSymbol(_producer)
        .appendToken(_deducer)
        .appendSymbol(_produced)
        .appendSymbol(_orProduced)
        .appendToken(_delimiter);
    syn.addProduction(_rule,std::move(prod));
    prod.clear();


    //4 <_producer> ::= <_VnExpr>;
    prod.appendSymbol(_VnExpr);
    syn.addProduction(_producer,std::move(prod));
    prod.clear();

    //5 <_produced> ::= <_produced> <_V> | null;
    prod.appendSymbol(_produced)
        .appendSymbol(_V);
    syn.addProduction(_produced,std::move(prod));
    prod.clear();

    syn.addProduction(_produced,std::move(prod));

    //6  <_orProduced> ::= _or <_produced> <_orProduced> | null ;
    prod.appendToken(_or)
        .appendSymbol(_produced)
        .appendSymbol(_orProduced);
    syn.addProduction(_orProduced,std::move(prod));
    prod.clear();

    syn.addProduction(_orProduced,std::move(prod));

    //7 <_V> ::= <_VnExpr> | <_Vt> ;
    prod.appendSymbol(_VnExpr);
    syn.addProduction(_V,std::move(prod));
    prod.clear();
    prod.appendSymbol(_Vt);
    syn.addProduction(_V,std::move(prod));
    prod.clear();

    //8 <_VnExpr> ::= _beforeVn <_Vn> _afterVn;
    prod.appendToken(_beforeVn)
        .appendSymbol(_Vn)
        .appendToken(_afterVn);
    syn.addProduction(_VnExpr,std::move(prod));
    prod.clear();

    //9 <_Vn> ::= _identifier;
    prod.appendToken(_identifier);
    syn.addProduction(_Vn,std::move(prod));
    prod.clear();

    //10 <_Vt> ::= _identifier | _identifier _space | _kwNull | _stringConst | _stringConst _space;
    prod.appendToken(_identifier);
    syn.addProduction(_Vt,std::move(prod));
    prod.clear();
    prod.appendToken(_identifier)
        .appendToken(_space);
    syn.addProduction(_Vt,std::move(prod));
    prod.clear();
    prod.appendToken(_kwNull);
    syn.addProduction(_Vt,std::move(prod));
    prod.clear();
    prod.appendToken(_stringConst);
    syn.addProduction(_Vt,std::move(prod));
    prod.clear();
    prod.appendToken(_stringConst)
        .appendToken(_space);
    syn.addProduction(_Vt,std::move(prod));
    prod.clear();

    lexer.toStream(std::wcout,tokenTable);
    syn.toStream(std::wcout,symbolTable,tokenTable);

    pGrammarParser = new LR0Grammar(LR0Grammar::analyze(syn));

    symbolTable.insert(L"ExtendedStart",_Vt+1);
}
