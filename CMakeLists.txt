cmake_minimum_required(VERSION 3.20)

project (scintilla-qt-demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS Widgets PrintSupport Core5Compat Xml)
include(CPM.cmake)
CPMAddPackage("gh:elcuco/scintilla-code#b6ab9e51106f7262e77b31c3eb5ca8a4d95c2fc7")
CPMAddPackage("gh:elcuco/lexilla#0500b7462bb2307fbc954c711a63f882ff648667")

add_executable(scintilla-qt-demo
    src/LexEditor.cpp
    src/LexEditor.h
    src/mainwindow.h
    src/mainwindow.cpp
    src/mainwindow.ui
    src/main.cpp
)

target_link_libraries(scintilla-qt-demo PUBLIC Qt::Widgets Qt::Xml scintilla-qt-edit lexzilla )
