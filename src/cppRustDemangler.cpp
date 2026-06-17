//
// Created by finns on 18.04.26.
//

#include "cppRustDemangler.h"

#include <cxxabi.h>
#include <memory>
#include <rustc_demangle.h>


std::string cppRustDemangler::demangle(const std::string &name) {
    if (name.starts_with("_R")) {
        return rustDemangle(name);
    }
    if (name.starts_with("_ZN")) {
        std::string cmp = rustDemangle(name);
        if (cmp != name) return cmp;
        cmp = cppDemangle(name);
        if (cmp != name) return cmp;
        return name;
    }

    if (!name.starts_with("_Z")) {
        return name;
    }
    if (std::string cmp = cppDemangle(name); cmp!=name) {
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

std::string cppRustDemangler::rustDemangle(const std::string &name) {
    char buf[4096];
    if (rustc_demangle(name.c_str(), buf, sizeof(buf))) {
        return {buf};
    }
    return name;
}
