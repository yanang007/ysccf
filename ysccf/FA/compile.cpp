#include <map>
#include "compile.h"

inline const static charType char_nochar = 0;
inline const static charType operator_nop = 0;
inline const static charType operator_join = 1;
inline const static charType operator_joinInStack = 2;
inline const static charType operator_kleeneStar = 3;
inline const static charType operator_kleeneCross = 4;
inline const static charType operator_optional = 5;
inline const static charType operator_cleaner = 6;

stringType escapedCharSet( charType escaped )
{
    static std::map<charType, stringType> mapping;
    if (mapping.size() == 0) {
        mapping['\\'] = '\\';
        mapping['?'] = '?';
        mapping['+'] = '+';
        mapping['*'] = '*';
        mapping['|'] = '|';
        mapping['('] = '(';
        mapping[')'] = ')';
        mapping['n'] = L"\n";
        mapping['r'] = L"\r";
        mapping['s'] = L"\f\n\r\t\v ";
        mapping['d'] = L"1234567890";
        mapping['L'] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        mapping['l'] = L"abcdefghijklmnopqrstuvwxyz";
        mapping['w'] = mapping['d'] + mapping['L'] + mapping['l'] + L"_";
        auto& allChars = mapping['a'];
        /*
        for(unsigned char i = 32; i < 126+1; ++i ){
            allChars.push_back(i);
        }*/
        allChars = mapping['w'];
    }
    try {
        return mapping.at(escaped);
    } catch (std::out_of_range e) {
        return L"";
    }
}

bool isOperator(charType a){
    switch (a)
    {
    //case '\\':
    case '?':
    case '+':
    case '*':
    case '|':
    case '(':
    case ')':
    case operator_join:
    case operator_joinInStack:
    case operator_cleaner:
    case operator_kleeneStar:
    case operator_kleeneCross:
    case operator_optional:
        return true;

    default:
        break;
    }
    return false;
}

int opPriority(charType c)
{
    static std::map<charType, int> mapping;
    if (mapping.size() == 0) {
        //initializing
        mapping['?'] = 4;
        mapping['+'] = 4;
        mapping['*'] = 4;
        mapping['|'] = 1;
        mapping[operator_join] = 3;
        mapping[operator_joinInStack] = 2;
        mapping[operator_kleeneStar] = 10;
        mapping[operator_kleeneCross] = 10;
        mapping[operator_optional] = 10;
        mapping[operator_cleaner] = 0; //用来清空运算符栈
        //mapping['\\'] = 12;  //转义运算符

        mapping['('] = 5;

        mapping[')'] = 0;
    }
    try {
        return mapping.at(c);
    } catch (std::out_of_range e) {
        return 1000;
    }
}

charType opConv(charType a) {
    switch (a)
    {
    default:
        return a;

    case '*':
        return operator_kleeneStar;
    case '+':
        return operator_kleeneCross;
    case '?':
        return operator_optional;

    case operator_joinInStack:
    case operator_join:
        return operator_joinInStack;

    case '(':
    case ')':
        return ')';
    }
}

nodePairType convertToMinimalGraph(charType ch,NFA& nfa)
{
    auto node1 = nfa.addNode();
    auto node2 = nfa.addNode();
    nfa.addPath(node1,node2,ch);
    return std::make_pair(node1,node2);
}

nodePairVecType requireParam(
        size_t n,
        std::stack<nodePairType>& resultStack )
{
    nodePairVecType ret;
    ret.reserve(n);

    nodeType node1 = 0, node2;
    while ( !resultStack.empty() && n != 0 ) {
        auto nodePair = toppop(resultStack);

        std::tie(node1,node2) = nodePair;

        ret.push_back(nodePair);
        --n;
    }
    if( n > 0){
        std::cout<<"requireParam: error: no more param can be popped! \n"<<n<<" more needed!\n";
    }
    return ret;
}

bool doCalc(
        charType op,
        charType& incentive,
        std::pair<charType,char_type> lookback,
        std::pair<charType,char_type> lookahead,
        std::stack<charType>& operatorStack,
        std::stack<nodePairType>& resultStack,
        NFA& nfa )
{
    bool insufficientParam = false;
    size_t paramAmount = 0;
    nodePairVecType params;
    decltype (params.begin()) paramIter;
    decltype (params.rbegin()) rparamIter;
    nodePairType pair1,pair2,midResultPair;
    nodeType node1,node2,node3;

    //auto op = toppop(operatorStack);

    switch ( op ) {
    case operator_joinInStack:
        paramAmount = 1; //已经弹出的自己也要算进去
        if( incentive == operator_joinInStack ){
            ++paramAmount;
        }
        while ( !operatorStack.empty() &&
                operatorStack.top() == operator_joinInStack ) {
            operatorStack.pop();
            ++paramAmount;
        }
        params = requireParam(paramAmount,resultStack);
        //请求2个参数
        if(params.size() < paramAmount){
            insufficientParam = true;
            break;
        }
        rparamIter = params.rbegin();
        pair2 = pair1 = *rparamIter; //第一个子图
        midResultPair.first = pair1.first;

        for( ++rparamIter ; rparamIter < params.rend(); ++rparamIter ){
            pair2 = *rparamIter;  //第二个子图
            nfa.addPath(pair1.second, pair2.first, eps);
            //用空字符连接
            pair1 = pair2;
        }

        midResultPair.second = pair2.second;

        break;

    case operator_kleeneStar:
        paramAmount = 1;
        params = requireParam(paramAmount,resultStack);
        if(params.size() < paramAmount){
            insufficientParam = true;
            break;
        }
        paramIter = params.begin();
        pair1 = *paramIter; //第一个子图

        node1 = nfa.addNode();
        node2 = nfa.addNode();

        nfa.addPath(node1, pair1.first, eps);
        nfa.addPath(pair1.second, node2, eps);
        nfa.addPath(node1, node2, eps);
        nfa.addPath(pair1.second, pair1.first, eps);

        midResultPair = std::make_pair(node1,node2);
        break;


    case operator_kleeneCross:
        paramAmount = 1;
        params = requireParam(paramAmount,resultStack);
        if(params.size() < paramAmount){
            insufficientParam = true;
            break;
        }
        paramIter = params.begin();
        pair1 = *paramIter; //第一个子图

        node1 = nfa.addNode();
        node2 = nfa.addNode();

        nfa.addPath(node1, pair1.first, eps);
        nfa.addPath(pair1.second, node2, eps);
        //nfa.addPath(node1, node2, eps);
        nfa.addPath(pair1.second, pair1.first, eps);

        midResultPair = std::make_pair(node1,node2);
        break;

    case operator_optional:
        paramAmount = 1;
        params = requireParam(paramAmount,resultStack);
        if(params.size() < paramAmount){
            insufficientParam = true;
            break;
        }
        paramIter = params.begin();
        pair1 = *paramIter; //第一个子图

        node1 = nfa.addNode();
        node2 = nfa.addNode();

        nfa.addPath(node1, pair1.first, eps);
        nfa.addPath(pair1.second, node2, eps);
        nfa.addPath(node1, node2, eps);

        midResultPair = std::make_pair(node1,node2);
        break;

    case '|':
        paramAmount = 2;
        params = requireParam(paramAmount,resultStack);
        //请求2个参数
        if(params.size() < paramAmount){
            insufficientParam = true;
            break;
        }

        node1 = nfa.addNode();
        node2 = nfa.addNode();

        for( auto pairn : params ){
            nfa.addPath(node1, pairn.first, eps);
            nfa.addPath(pairn.second, node2, eps);
        }

        midResultPair = std::make_pair(node1,node2);

        break;

    case ')':
        paramAmount = 0;
        if(incentive != ')'){
            //导致 ")" 弹出
            //诱因运算符必须为 ")"
            // 注，左括号 会在进栈时转化为 右括号
            return false;
            //error
        }
        incentive = operator_joinInStack; //将诱因运算符置为连接符进栈
        return false;

    default:
        break;
    }

    if( insufficientParam ){
        std::cout<<"doCalc: error: not enough parameter for operator id:" + std::to_string((short)op);
    }

    resultStack.push(midResultPair);
    return true;
}

void pushOperator(
        charType op,
        std::pair<charType,char_type> lookback,
        std::pair<charType,char_type> lookahead,
        std::stack<charType>& operatorStack,
        std::stack<nodePairType>& resultStack,
        NFA& nfa  )
{
    while (!operatorStack.empty() && opPriority(op) <= opPriority(operatorStack.top())) {
        //大于等于还是大于应取决与运算符的运算方向
        //从左至右则为大于等于，同级运算符中先入栈的也会被优先计算
        //此处运算方向一律设置为左结合
        //if (op == ')' && operatorStack.top() == ')') break;
        //os << ss.pop();
        if( !doCalc(toppop(operatorStack), op,// 要处理的运算符，和弹栈行为的起因
               lookback,lookahead,
               operatorStack,resultStack,nfa) )
            break;
    }
    if( op != operator_nop ){
        operatorStack.push(opConv(op));
    }
}

std::pair<charType,char_type>
        fetch(const stringType& expr,
              stringType::const_iterator& ich)
{
    auto ret = std::make_pair(char_nochar,ctNoChar);
    if( ich == expr.cend() ){
        return ret;
    }
    if (*ich == '\\') {
        ++ich;
        if( ich != expr.cend() )
        {
            ret.first = *ich;
            ret.second = ctEscaped;
        }
    }
    else{
        ret.first = *ich;
        ret.second = isOperator(ret.first) ? ctOperator : ctChar;
    }
    if( ich != expr.cend() ){
        ++ich;
    }

    return ret;
}

NFA compile(const stringType& expr)
{
    NFA nfa;
    std::stack<charType> operatorStack;
    std::stack<nodePairType> resultStack;

    std::pair<charType,char_type>
            ch,
            lookahead,
            lookback = std::make_pair(char_nochar,ctNoChar);
    nodeType start, terminal;

    auto ich = expr.cbegin();
    lookahead = fetch(expr,ich);

    for( ; lookahead.second != ctNoChar ; ){
        ch = lookahead;
        lookahead = fetch(expr,ich);
        if ( ch.second == ctOperator ) {
            pushOperator(ch.first,lookback,lookahead,operatorStack,resultStack,nfa);
        }
        else if ( ch.second == ctChar ) {
            pushOperator(operator_join,lookback,lookahead,operatorStack,resultStack,nfa);
            resultStack.push(convertToMinimalGraph(ch.first,nfa));
                //operatorStack.push(operator_join);
        }
        else if ( ch.second == ctEscaped ) {
            nodeType b = nfa.addNode();
            nodeType e = nfa.addNode();
            for ( const auto _ch : escapedCharSet(ch.first)){
                nfa.addPath(b,e,_ch);
            }
            pushOperator(operator_join,lookback,lookahead,operatorStack,resultStack,nfa);
            resultStack.push(std::make_pair(b,e));
        }
        else{
            //error
        }
        lookback = ch;
    }

    pushOperator(operator_cleaner,lookback,lookahead,operatorStack,resultStack,nfa);

    if( resultStack.size() == 1 ){
        //success
        std::tie(start,terminal) = toppop(resultStack);
        nfa.markStart(start);
        nfa.markTerminal(terminal);
    }
    else if( resultStack.size() < 1 ){
        auto temp = nfa.addNode(); //0
        nfa.markStart(temp);
        nfa.markTerminal(temp);
    }

    return nfa;
}

DFA directCompile(const stringType &str)
{
    DFA ret;
    nodeType end = ret.addNode(), temp;
    for(const auto& ch : str){
        temp = ret.addNode();
        ret.addPath(end,temp,ch);
        end = temp;
    }
    ret.markTerminal(end);
    return ret;
}
