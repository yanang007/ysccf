#ifndef TLEXER_H
#define TLEXER_H

#include "../base/base.h"
//#define _USE_QLEXER
//#define _USE_LEXER

#ifdef _USE_QT
#include "./qLexer.h"
using tLexer = qLexer;
#elif defined (_USE_LEXER)
#include "./lexer.h"
using tLexer = lexer;
#else
#include "./cLexer.h"
using tLexer = cLexer;
#endif


#endif // TLEXER_H
