cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project (ysccf)

# Qt support
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# options
option(_SYNTAXTEST "run syntax parsing test" ON)
option(_REGTEST "run DFA test" OFF)
option(_USE_QT "use Qt(using QRegex as lexer's backend)" OFF)
option(_USE_QWINDOW "use Qt GUI to demonstrate" OFF)
configure_file(
    "${PROJECT_SOURCE_DIR}/base/configs/config.h.in"
    "${PROJECT_SOURCE_DIR}/base/configs/config.h"
    )

# find source files
file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})

# generate filters for VS project
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_files})

# add build target
add_executable(${PROJECT_NAME} ${all_files})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)