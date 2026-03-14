cmake_minimum_required(VERSION 4.1)
project(Disassembly-Viewer)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "~/Qt/6.10.1/gcc_64/lib/cmake/Qt6")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAPSTONE REQUIRED capstone)

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
        src/Disassembler.h
        src/x86_64Disasm.cpp
        src/x86_64Disasm.h
        gui/widgets/membar.cpp
        gui/widgets/membar.h
        gui/widgets/membar.ui
)
target_include_directories(Disassembly-Viewer PRIVATE
        ${CMAKE_SOURCE_DIR}/gui
        ${CMAKE_SOURCE_DIR}/gui/widgets
        ${CMAKE_SOURCE_DIR}/src
        ${CAPSTONE_INCLUDE_DIRS}

)
target_link_libraries(Disassembly-Viewer
        Qt::Core
        Qt::Gui
        Qt::Widgets
        capstone
)