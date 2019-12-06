#include "errorReport.h"

#include <string>

void errorReport(stringType src, stringType type, int erno){
    std::wcout << src << L" : " << type << std::endl;
}
