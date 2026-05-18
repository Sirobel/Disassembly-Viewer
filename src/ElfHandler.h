//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_ELFHANDLER_H
#define DISASSEMBLY_VIEWER_ELFHANDLER_H

#pragma once
#include <string>
#include <elf.h>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Demangler.h"

class ElfHandler {
public:
    explicit ElfHandler(const std::string &path) {
        filePath = path;
    }

    virtual ~ElfHandler() = default;

    virtual std::vector<std::pair<uint64_t, std::string> > getSectionHeadersNames() {
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

    virtual std::vector<std::pair<uint64_t, uint64_t> > getLoadableProgramHeaders() {
        return {};
    }

    virtual std::vector<std::pair<uint64_t, uint64_t> > getLoadableSectionHeaders() {
        return {};
    }

    virtual std::vector<std::pair<std::string, Elf64_Shdr> > getSections64() {
        return {};
    }

    virtual std::vector<Elf64_Phdr> getProgramHeaders64() {
        return {};
    }

    virtual std::vector<std::pair<std::string, std::vector<char> > > getStringTables() {
        return {};
    }

    virtual std::vector<std::pair<std::string, std::pair<std::string, Elf64_Sym> > > getSymbolTablesElf64() {
        return {};
    }

    virtual std::unordered_map<std::string, std::pair<std::string, std::vector<std::pair<std::string, Elf64_Rela> > > >
    getRelaTables64() {
        return {};
    }

    virtual Elf64_Ehdr getElf64Header() {
        return {};
    }

protected:
    std::unique_ptr<Demangler> demangler;

    std::string demangle(const std::string &name) {
        return demangler->demangle(name);
    }

private:
    std::string filePath;
};


#endif //DISASSEMBLY_VIEWER_ELFHANDLER_H
