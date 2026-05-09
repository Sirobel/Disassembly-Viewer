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

include(ExternalProject)
set(RUSTC_DEMANGLE_SRC "${CMAKE_SOURCE_DIR}/third_party/rustc-demangle")
set(RUSTC_DEMANGLE_LIB "${RUSTC_DEMANGLE_SRC}/target/release/librustc_demangle.a")

ExternalProject_Add(rustc_demangle_build
        SOURCE_DIR      "${RUSTC_DEMANGLE_SRC}"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND   cargo build -p rustc-demangle-capi --release
        --manifest-path "${RUSTC_DEMANGLE_SRC}/Cargo.toml"
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE TRUE
        BUILD_BYPRODUCTS "${RUSTC_DEMANGLE_LIB}"
)

add_library(rustc_demangle STATIC IMPORTED GLOBAL)
add_dependencies(rustc_demangle rustc_demangle_build)
set_target_properties(rustc_demangle PROPERTIES
        IMPORTED_LOCATION             "${RUSTC_DEMANGLE_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${RUSTC_DEMANGLE_SRC}/crates/capi/include"
)

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
        gui/settings/mainsettings.cpp
        gui/settings/mainsettings.h
        gui/settings/mainsettings.ui
        gui/settings/recentfilessettings.cpp
        gui/settings/recentfilessettings.h
        gui/settings/recentfilessettings.ui
        gui/settings/textviewersettings.cpp
        gui/settings/textviewersettings.h
        gui/settings/textviewersettings.ui
        gui/textviewer/infos/fileinfo.cpp
        gui/textviewer/infos/fileinfo.h
        gui/textviewer/infos/fileinfo.ui
        gui/textviewer/infos/columinfo.cpp
        gui/textviewer/infos/columinfo.h
        gui/textviewer/infos/columinfo.ui
        gui/textviewer/infos/treeinfo.cpp
        gui/textviewer/infos/treeinfo.h
        gui/textviewer/infos/treeinfo.ui
        src/Demangler.h
        src/cppRustDemangler.cpp
        src/cppRustDemangler.h
        gui/textviewer/DisasmModel.cpp
        gui/textviewer/DisasmModel.h
        gui/textviewer/DisasmDelegate.cpp
        gui/textviewer/DisasmDelegate.h
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
        rustc_demangle
        pthread
        dl
)