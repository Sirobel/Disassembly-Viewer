//
// Created by finns on 18.04.26.
//

#ifndef DISASSEMBLY_VIEWER_DEMANGLER_H
#define DISASSEMBLY_VIEWER_DEMANGLER_H
#include <string>


class Demangler {
public:
    virtual ~Demangler() = default;

    virtual std::string demangle(const std::string &name) {
        return "";
    }
};


#endif //DISASSEMBLY_VIEWER_DEMANGLER_H
