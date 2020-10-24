#ifndef BASE_H
#define BASE_H

#include <set>
#include <vector>
#include <map>
#include <iostream>

#include "./configs/config.h"

typedef size_t nodeType;
typedef wchar_t charType;
typedef std::basic_string<charType> stringType;
typedef std::basic_ostream<charType> ostreamType;
typedef std::basic_istream<charType> istreamType;
typedef std::basic_iostream<charType> iostreamType;

static ostreamType& outStream = std::wcout;
static istreamType& inStream = std::wcin;

typedef std::set<nodeType> nodesetType;
typedef std::vector<nodeType> nodevecType;
typedef std::set<charType> charsetType;
typedef std::map<nodeType,size_t> nodeToSizeMapType;
typedef std::pair<nodeType,nodeType> nodePairType;
typedef std::vector<nodePairType> nodePairVecType;
typedef std::vector<size_t> sizeVecType;

constexpr charType eps = '\0';
constexpr nodeType nodeNotExist = static_cast<nodeType>(-1);
constexpr size_t notFound = static_cast<size_t>(-1);

#define UNUSED(expr) do { (void)(expr); } while (0)

inline bool isEps(char c){ return c==eps; }

#endif // BASE_H
