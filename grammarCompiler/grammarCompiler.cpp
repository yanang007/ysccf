#include "grammarCompiler.h"

#include "../base/errorReport.h"
#include "../utils/invokeful.hpp"


grammarCompiler::grammarCompiler()
{
    initGrammarCompiler();
}

grammarCompiler::~grammarCompiler()
{
}

lexer::tokenStreamStorage grammarCompiler::tokenize(stringType str)
{
    return compilerLexer().tokenize(str);
}

pSyntaxTree grammarCompiler::parse(const lexer::tokenStreamStorage& stream)
{
    return pGrammarParser->parse(stream);
}

pSyntaxTree grammarCompiler::parse(stringType input)
{
    return cfeOfCC.parse(input);
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

    return cfe.grammar();
}


void grammarCompiler::analyzeParser(pcSyntaxTree tree)
{
    cfe.clear();

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
        if(decRet == compilerFrontend::dsUndefined)
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
                        if( dsret == compilerFrontend::dsUndefined )
                            break;
                        prod.appendSymbol(nodeCont);
                        break;
                    case vttKwNull:
                        break;
                    case vttStringConst:
                        raw = replaced(raw,stringType(L"\\\""),stringType(L"\""));
                        std::tie(nodeCont,dsret) =
                                declareNewToken(stringType(L"_s\"")+raw+L"\"",raw,true);
                        if( dsret == compilerFrontend::dsUndefined )
                            break;
                        prod.appendToken(nodeCont);
                        break;
                    case vttVtIndtifier:
                        std::tie(nodeCont,dsret) =
                                declareNewToken(raw,stringType(L"诶？!"));
                        switch ( dsret ) {
                        case compilerFrontend::dsRedefined:
                            prod.appendToken(nodeCont);
                            break;
                        default:
                            errorReport(L"grammarCompiler::parseProduced()",
                                        stringType(L"token \"") + raw + L"\" not defined",
                                        ec_parser_TokenNotDefined);
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
    cfe.grammar().addProduction(producer,prod);
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
    return cfe.declareNewToken(name, str, escaped);
    /*if ( !name.empty() && !str.empty() ){
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
    return std::make_pair(nodeNotExist,dsUndefined);*/
}

std::pair<nodeType, grammarCompiler::declareState> grammarCompiler::declareNewSymbol(const stringType &name)
{
    return cfe.declareNewSymbol(name);
    /*if ( !name.empty() ){
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
    return std::make_pair(nodeNotExist,dsUndefined);*/
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
    std::tie(_kwNull, std::ignore) = cfeOfCC.declareNewToken(L"kwNull", LR"(null)");
    std::tie(_kwToken, std::ignore) = cfeOfCC.declareNewToken(L"kwToken", lexer::tokenDefiner);
    std::tie(_beforeVn, std::ignore) = cfeOfCC.declareNewToken(L"beforeVn", LR"(<)");
    std::tie(_afterVn, std::ignore) = cfeOfCC.declareNewToken(L"afterVn", LR"(>)");
    std::tie(_stringConst, std::ignore) = cfeOfCC.declareNewToken(L"stringConst", LR"("(\\.|[^"\\])*")");
    std::tie(_comment, std::ignore) = cfeOfCC.declareNewToken(L"comment", LR"(/\*.*\*/)");
    std::tie(_deducer, std::ignore) = cfeOfCC.declareNewToken(L"deducer", LR"(::=)");
    std::tie(_delimiter, std::ignore) = cfeOfCC.declareNewToken(L"delimiter", LR"(;)");
    std::tie(_or, std::ignore) = cfeOfCC.declareNewToken(L"or", LR"(\|)");
    std::tie(_space, std::ignore) = cfeOfCC.declareNewToken(L"space", LR"(\s*)");
    std::tie(_identifier, std::ignore) = cfeOfCC.declareNewToken(L"identifier", LR"([a-zA-Z_]\w*)");
    std::tie(_beforeAttr, std::ignore) = cfeOfCC.declareNewToken(L"beforeAttr", LR"(\[)");
    std::tie(_afterAttr, std::ignore) = cfeOfCC.declareNewToken(L"afterAttr", LR"(\])");

    cfeOfCC.setTokenToBeIgnored(_comment);
    cfeOfCC.setTokenToBeIgnored(_space);

    std::tie(_grammarDef, std::ignore) = cfeOfCC.declareNewSymbol(L"grammarDef");
    std::tie(_statement, std::ignore) = cfeOfCC.declareNewSymbol(L"statement");
    std::tie(_tokenDef, std::ignore) = cfeOfCC.declareNewSymbol(L"tokenDef");
    std::tie(_rule, std::ignore) = cfeOfCC.declareNewSymbol(L"rule");
    std::tie(_producer, std::ignore) = cfeOfCC.declareNewSymbol(L"producer");
    std::tie(_produced, std::ignore) = cfeOfCC.declareNewSymbol(L"produced");
    std::tie(_orProduced, std::ignore) = cfeOfCC.declareNewSymbol(L"orProduced");
    std::tie(_V, std::ignore) = cfeOfCC.declareNewSymbol(L"V");
    std::tie(_VnExpr, std::ignore) = cfeOfCC.declareNewSymbol(L"VnExpr");
    std::tie(_Vn, std::ignore) = cfeOfCC.declareNewSymbol(L"Vn");
    std::tie(_Vt, std::ignore) = cfeOfCC.declareNewSymbol(L"Vt");
    std::tie(_attribute, std::ignore) = cfeOfCC.declareNewSymbol(L"attribute");
    std::tie(_attributedStatement, std::ignore) = cfeOfCC.declareNewSymbol(L"attributedStatement");

    grammar syn;
    cfeOfCC.markStart(_grammarDef);

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
<tokenDef> ::= kwToken identifier stringConst ;  /// space deleted, need inspection
<rule> ::= <producer> deducer <produced> <orProduced> delimiter ;
<producer> ::= <VnExpr> ;
<produced> ::= <produced> <V> | null;
<orProduced> ::= or <produced> <orProduced> | null;
<V> ::= <VnExpr> | <Vt> ;
<VnExpr> ::= beforeVn <Vn> afterVn ;
<Vn> ::= identifier ;
<Vt> ::= identifier | identifier | kwNull | stringConst | stringConst ;  /// space deleted, need inspection
<ExtendedStart> ::= <grammarDef> ;
)kk";

    //0 <_grammarDef> ::= <_grammarDef> <_statement> | null;
    grammar::productionID launcher;
    cfeOfCC.grammar().addProduction(_grammarDef, &launcher)
        .appendSymbol(_grammarDef)
        .appendSymbol(_statement);

    cfeOfCC.grammar().addProduction(_grammarDef);

    //1 <_statement> ::= <_tokenDef> | <_rule> | _comment ;
    cfeOfCC.grammar().addProduction(_statement)
        .appendSymbol(_tokenDef);

    cfeOfCC.grammar().addProduction(_statement)
        .appendSymbol(_rule);

    cfeOfCC.grammar().addProduction(_statement)
        .appendToken(_comment);

    //2 <_tokenDef> ::= _kwToken _identifier _stringConst;
    cfeOfCC.grammar().addProduction(_tokenDef)
        .appendToken(_kwToken)
        .appendToken(_identifier)
        .appendToken(_stringConst);

    //3 <_rule> ::= <_producer> _deducer <_produced> <_orProduced> _delimiter ;
    cfeOfCC.grammar().addProduction(_rule)
        .appendSymbol(_producer)
        .appendToken(_deducer)
        .appendSymbol(_produced)
        .appendSymbol(_orProduced)
        .appendToken(_delimiter);


    //4 <_producer> ::= <_VnExpr>;
    cfeOfCC.grammar().addProduction(_producer)
        .appendSymbol(_VnExpr);

    //5 <_produced> ::= <_produced> <_V> | null;
    cfeOfCC.grammar().addProduction(_produced)
        .appendSymbol(_produced)
        .appendSymbol(_V);

    cfeOfCC.grammar().addProduction(_produced);

    //6  <_orProduced> ::= _or <_produced> <_orProduced> | null ;
    cfeOfCC.grammar().addProduction(_orProduced)
        .appendToken(_or)
        .appendSymbol(_produced)
        .appendSymbol(_orProduced);

    cfeOfCC.grammar().addProduction(_orProduced);

    //7 <_V> ::= <_VnExpr> | <_Vt> ;
    cfeOfCC.grammar().addProduction(_V)
        .appendSymbol(_VnExpr);

    cfeOfCC.grammar().addProduction(_V)
        .appendSymbol(_Vt);

    //8 <_VnExpr> ::= _beforeVn <_Vn> _afterVn;
    cfeOfCC.grammar().addProduction(_VnExpr)
        .appendToken(_beforeVn)
        .appendSymbol(_Vn)
        .appendToken(_afterVn);

    //9 <_Vn> ::= _identifier;
    cfeOfCC.grammar().addProduction(_Vn)
        .appendToken(_identifier);

    //10 <_Vt> ::= _identifier | _identifier | _kwNull | _stringConst | _stringConst;
    cfeOfCC.grammar().addProduction(_Vt)
        .appendToken(_identifier);

    cfeOfCC.grammar().addProduction(_Vt)
        .appendToken(_identifier);

    cfeOfCC.grammar().addProduction(_Vt)
        .appendToken(_kwNull);

    cfeOfCC.grammar().addProduction(_Vt)
        .appendToken(_stringConst);

    cfeOfCC.grammar().addProduction(_Vt)
        .appendToken(_stringConst);

    cfeOfCC.toLexerStream(std::wcout);
    cfeOfCC.toGrammarStream(std::wcout);

    cfeOfCC.generateParser();
    pGrammarParser = &cfeOfCC.parser();

    //symbolTable.insert(L"ExtendedStart",_Vt+1);
}
