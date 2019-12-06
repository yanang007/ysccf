#ifndef FAVIZ_H
#define FAVIZ_H

#include <sstream>
#include <functional>
#include <fstream>
#include <codecvt>

#include "../utils/utils.hpp"
#include "../FA/FA.h"

inline
std::wstring nodeLabelDefault(nodeType n)
{
    return std::to_wstring(n);
}

inline
std::wstring pathLabelDefault(charType path)
{
    if ( path == eps ) {
        return std::wstring(L"ε");
    }
    else {
        return std::wstring(1,path);
    }
}

template <typename TT,hiddenDetect IH>
void vizFA(
        std::string exePath,
        std::string outputFormat,
        std::string name,
        FA<TT,IH> fa,
        std::string extra_flags = "",
        std::function<std::wstring(nodeType)> nodeLabel = nodeLabelDefault,
        std::function<std::wstring(charType)> pathLabel = pathLabelDefault,
        std::wstring terminalNodeColor = L"red",
        std::wstring startNodeColor = L"green",
        std::wstring normalNodeColor = L"black"
        )
{
    const static
    std::string dotSuffix(".dot");

    auto filestr = vizFAImpl(
                fa,nodeLabel,pathLabel,
                terminalNodeColor,
                startNodeColor,
                normalNodeColor );

    std::wofstream os(name+dotSuffix,std::ios_base::out);
    //os.imbue(loc);

    os<<filestr;
    os.close();

    auto cmd = exePath +
               " -T" + outputFormat + " " +
               name + dotSuffix + " -O " + extra_flags;
    system(cmd.c_str());
}

template <typename TT,hiddenDetect IH>
std::wstring vizFAImpl(
        FA<TT,IH> fa,
        std::function<std::wstring(nodeType)> nodeLabel = nodeLabelDefault,
        std::function<std::wstring(charType)> pathLabel = pathLabelDefault,
        std::wstring terminalNodeColor = L"red",
        std::wstring startNodeColor = L"green",
        std::wstring normalNodeColor = L"black"
        )
{
    const static
    std::wstring fontConfig(LR"(edge [fontname="FangSong"];)" L"\n"
                           LR"(node [fontname="FangSong"])");
    std::wstringstream vizstr;
    vizstr<<"digraph {"<<std::endl;
    vizstr<<fontConfig<<std::endl;
    std::wstring color;
    for( const auto& node : fa.overviewAsVector() ){
        if ( node == fa.start() )
            color = startNodeColor;
        else if ( contains(fa.terminals(),node) )
            color = terminalNodeColor;
        else
            color = normalNodeColor;

        //0 [label=0 color=black]
        vizstr<<"\t"
              << node<<" "
              <<"["
              <<"label="<<nodeLabel(node)<<" "
              <<"color="<<color<<"]"
              << std::endl;
    }

    size_t nodeID = 0;
    for(const auto &node : fa.nodes())
    {
        //0 -> 1 [label=a]
        for(const auto &road : node)
        {
            for(const auto& dest : road.second){
                vizstr<<"\t"
                      << nodeID
                      <<" -> "
                      << dest<<" "
                      <<"[label=\""
                      <<pathLabel(road.first)
                      <<"\"]"
                      <<std::endl;
            }
        }
        ++nodeID;
    }

    vizstr<<"}";
    return vizstr.str();
}



#endif //FAVIZ_H
