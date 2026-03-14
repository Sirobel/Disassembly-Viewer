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

    virtual std::vector<std::pair<uint64_t,std::string>> getSectionHeadersNames() {
        return {};
    }

    virtual std::vector<uint8_t> getSection(const std::string &sectionName) {
        return {};
    }

    virtual std::string lookupRangeSymbol(uint64_t addr) {
        return "";
    }
    virtual std::string lookupSymbol(uint64_t addr) {
        return "";
    }

    virtual uint64_t getAddressOfSegment(const std::string &segmentName) {
        return 0;
    }

    virtual std::vector<std::pair<uint64_t, uint64_t>> getLoadableProgramHeaders() {
        return {};
    }
    virtual std::vector<std::pair<uint64_t, uint64_t>> getLoadableSectionHeaders() {
        return {};
    }

private:
    std::string filePath;
};


#endif //DISASSEMBLY_VIEWER_ELFHANDLER_H
