//
// Created by finns on 18.04.26.
//

#include "cppRustDemangler.h"

#include <cstring>
#include <cxxabi.h>
#include <memory>


std::string cppRustDemangler::demangle(const std::string &name) {
    if (name.find("_Z") != 0) {
        return name;
    }
    if (std::string cmp = cppDemangle(name); strcmp(cmp.c_str(), name.c_str()) != 0) {
        return cmp;
    }

    return name;
}

std::string cppRustDemangler::cppDemangle(const std::string &name) {
    int status = 0;
    const std::unique_ptr<char, void(*)(void *)> demangled(
        abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status)
        , std::free);

    if (status == 0 && demangled) {
        return {demangled.get()};
    }
    return name;
}
