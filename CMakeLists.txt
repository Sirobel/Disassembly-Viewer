cmake_minimum_required(VERSION 4.1)
project(Disassembly-Viewer)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/finns/Qt/6.10.1/gcc_64/lib/cmake/Qt6")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Disassembly-Viewer src/main.cpp
        gui/mainwindow/windowhandler.cpp
        gui/mainwindow/windowhandler.h
        gui/mainwindow/windowhandler.ui
        gui/textviewer/textviewer.cpp
        gui/textviewer/textviewer.h
        gui/textviewer/textviewer.ui
        gui/startwindow/recentfiles.cpp
        gui/startwindow/recentfiles.h
        gui/startwindow/recentfiles.ui
        src/ElfHandler.h
        src/x86_64elf.cpp
        src/x86_64elf.h
)
target_include_directories(Disassembly-Viewer PRIVATE
        ${CMAKE_SOURCE_DIR}/gui
        ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(Disassembly-Viewer
        Qt::Core
        Qt::Gui
        Qt::Widgets
)