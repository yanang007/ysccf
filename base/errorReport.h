#ifndef ERRORREPORT_H
#define ERRORREPORT_H

#include "base.h"

enum error_code {
    ec_Lexer_UnknownToken = 10,
    ec_parser_TokenNotDefined = 40
};

void errorReport(stringType src, stringType type, int erno);

#endif // ERRORREPORT_H
