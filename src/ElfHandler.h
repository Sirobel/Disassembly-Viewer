//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_ELFHANDLER_H
#define DISASSEMBLY_VIEWER_ELFHANDLER_H

#pragma once
#include <string>
#include <elf.h>
#include <fstream>
#include <vector>

class ElfHandler {
public:
    explicit ElfHandler(const std::string &path) {
        filePath = path;
    }

    virtual ~ElfHandler() = default;

    virtual std::vector<std::string> getSectionHeadersNames() {
        return {};
    }

private:
    std::string filePath;

};


#endif //DISASSEMBLY_VIEWER_ELFHANDLER_H