//
// Created by finns on 18.04.26.
//

#ifndef DISASSEMBLY_VIEWER_CPPRUSTDEMANGLER_H
#define DISASSEMBLY_VIEWER_CPPRUSTDEMANGLER_H
#include "Demangler.h"


class cppRustDemangler : public Demangler {
public:

    std::string demangle(const std::string &name) override;

private:
    static std::string cppDemangle(const std::string &name);
    static std::string rustDemangle(const std::string& name);
};


#endif //DISASSEMBLY_VIEWER_CPPRUSTDEMANGLER_H
