//
// Created by finns on 09.04.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_treeinfo.h" resolved

#include "treeinfo.h"
#include "ui_treeinfo.h"


treeinfo::treeinfo(QWidget *parent) : QWidget(parent), ui(new Ui::treeinfo) {
    ui->setupUi(this);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

treeinfo::~treeinfo() {
    delete ui;
}

void treeinfo::setDisplayInfo(const int index) {
    ui->treeWidget->clear();

    switch (index) {
        case 2:
            showStringTables();
            break;
        case 3:
            showSymbolTables();
            break;
        case 4:
            showRelocationTables();
            break;
        case 5:
            showProgramTable();
            break;
        default:
            break;
    }
}

void treeinfo::setStringTable(const QHash<QString, QVector<QString> > &table) {
    stringTables = table;
}

void treeinfo::setSymbolTable(const QHash<QString, QList<QPair<QString, Elf64_Sym> > > &table) {
    symbolTables = table;
}

void treeinfo::setRelocationTable(const QHash<QString, QPair<QString, QList<QPair<QString, Elf64_Rela> > > > &tables) {
    relocationTables = tables;
}

void treeinfo::setProgramHeader(const QVector<Elf64_Phdr> &data) {
    programHeaders = data;
}

void treeinfo::setSections(const QVector<QPair<QString, Elf64_Shdr> > &data) {
    sections = data;
}

void treeinfo::showStringTables() {
    ui->treeWidget->setColumnCount(static_cast<int>(stringTableColumn.count()));
    ui->treeWidget->setHeaderLabels(stringTableColumn);

    for (const auto &[section, strings]: stringTables.asKeyValueRange()) {
        const auto item = new QTreeWidgetItem(ui->treeWidget, QStringList{section});

        for (const auto &string: strings) {
            new QTreeWidgetItem(item, QStringList{string});
        }
    }
}

void treeinfo::showSymbolTables() {
    ui->treeWidget->setColumnCount(static_cast<int>(symbolTableColumn.count()));
    ui->treeWidget->setHeaderLabels(symbolTableColumn);

    for (const auto &[section, strings]: symbolTables.asKeyValueRange()) {
        auto item = new QTreeWidgetItem(ui->treeWidget, QStringList{section});
        for (const auto &[string,symbol]: strings) {
            QStringList names = {string};

            names.append([&] {
                switch (ELF64_ST_TYPE(symbol.st_info)) {
                    case STT_NOTYPE:
                        return "unspecified";
                    case STT_OBJECT:
                        return "data object";
                    case STT_FUNC:
                        return "code object";
                    case STT_SECTION:
                        return "section";
                    case STT_FILE:
                        return "file";
                    case STT_COMMON:
                        return "common data object";
                    case STT_TLS:
                        return "thread local data object";
                    case STT_LOOS:
                        return "start of os-specific/ GNU indirect code object";
                    case STT_HIOS:
                        return "end of os-specific";
                    case STT_LOPROC:
                        return "start of processor-specific";
                    case STT_HIPROC:
                        return "end of processor-specific";
                    default:
                        return "unknown";
                }
            }());

            names.append([&] {
                switch (ELF64_ST_BIND(symbol.st_info)) {
                    case STB_LOCAL:
                        return "local";
                    case STB_GLOBAL:
                        return "global";
                    case STB_WEAK:
                        return "weak";
                    case STB_LOOS:
                        return "start of OS-specific / GNU unique";
                    case STB_HIOS:
                        return "end of OS-specific";
                    case STB_LOPROC:
                        return "start of processor-specific";
                    case STB_HIPROC:
                        return "end of processor-specific";
                    default:
                        return "unknown";
                }
            }());
            names.append([&] {
                switch (ELF64_ST_VISIBILITY(symbol.st_other)) {
                    case STV_DEFAULT:
                        return "default";
                    case STV_INTERNAL:
                        return "processor specific";
                    case STV_HIDDEN:
                        return "hidden/unavailabe in other modules";
                    case STV_PROTECTED:
                        return "Not preemptible, not exported";
                    default:
                        return "unknown";
                }
            }());

            new QTreeWidgetItem(item, names);
        }
    }
}

void treeinfo::showRelocationTables() {
    ui->treeWidget->setColumnCount(static_cast<int>(relocationTableColumn.count()));
    ui->treeWidget->setHeaderLabels(relocationTableColumn);

    for (const auto &[section, strings]: relocationTables.asKeyValueRange()) {
        auto item = new QTreeWidgetItem(ui->treeWidget, QStringList{section});
        auto symbolTable = new QTreeWidgetItem(item, QStringList{strings.first});

        for (const auto &[string, rela]: strings.second) {
            QStringList names = {string};
            names.append(QString::number(rela.r_offset));
            names.append(QString::number(rela.r_addend));
            ////for later in the elf.h under /* AMD x86-64 relocations.  */
            names.append([&] {
                switch (ELF64_R_TYPE(rela.r_info)) {
                    case R_X86_64_NONE:
                        return "no reloc";
                    case R_X86_64_64:
                        return "direct 64 bit";
                    case R_X86_64_PC32:
                        return "pc relative 32 bit signed";
                    case R_X86_64_GOT32:
                        return "32 bit got entry";
                    case R_X86_64_PLT32:
                        return "32 bit plt address";
                    case R_X86_64_COPY:
                        return "copy symbol at runtime";
                    case R_X86_64_GLOB_DAT:
                        return "create got entry";
                    case R_X86_64_JUMP_SLOT:
                        return "create plt entry";
                    case R_X86_64_RELATIVE:
                        return "adjust by program base";
                    case R_X86_64_GOTPCREL:
                        return "32 bit signed pc relative offset to got";
                    case R_X86_64_32:
                        return "direct 32 bit zero extend";
                    case R_X86_64_32S:
                        return "direct 32 bit signed extend";
                    case R_X86_64_16:
                        return "direct 16 bit zero extend";
                    case R_X86_64_PC16:
                        return "direct 16 bit sign extended pc relative";
                    case R_X86_64_8:
                        return "direct 8 bit sign extend";
                    case R_X86_64_PC8:
                        return "8 bit sign extend pc relative";
                    case R_X86_64_DTPMOD64:
                        return "ID of module containing symbol";
                    case R_X86_64_DTPOFF64:
                        return "offset in module's TLS block";
                    case R_X86_64_TPOFF64:
                        return "offset in initial TLS block";
                    case R_X86_64_TLSGD:
                        return "32 bit signed PC relative offset to two GOT entries for GD symbol";
                    case R_X86_64_TLSLD:
                        return "32 bit signed PC relative offset to two GOT entries for LD symbol";
                    case R_X86_64_DTPOFF32:
                        return "offset in TLS block";
                    case R_X86_64_GOTTPOFF:
                        return "32 bit signed PC relative offset to GOT entry for IE symbol";
                    case R_X86_64_TPOFF32:
                        return "offset in initial TLS block";
                    case R_X86_64_PC64:
                        return "pc relative 64 bit";
                    case R_X86_64_GOTOFF64:
                        return "64 bit offset to GOT";
                    case R_X86_64_GOTPC32:
                        return "2 bit signed pc relative offset to GOT";
                    case R_X86_64_GOT64:
                        return "64 bit GOT entry offset";
                    case R_X86_64_GOTPCREL64:
                        return "64 bit pc relative offset to GOT entry";
                    case R_X86_64_GOTPC64:
                        return "64 bit pc relative offset to GOT";
                    case R_X86_64_GOTPLT64:
                        return "64 bit pc relative offset to GOT, says PLT entry needed";
                    case R_X86_64_PLTOFF64:
                        return "64 bit GOT relative offset to PLT entry";
                    case R_X86_64_SIZE32:
                        return "size of symbol plus 32 bit addend";
                    case R_X86_64_SIZE64:
                        return "size of symbol plus 64 bit addend";
                    case R_X86_64_GOTPC32_TLSDESC:
                        return "GOT offset for TLS descriptor";
                    case R_X86_64_TLSDESC_CALL:
                        return "Marker for call through TLS descriptor";
                    case R_X86_64_TLSDESC:
                        return "TLS descriptor";
                    case R_X86_64_IRELATIVE:
                        return "Adjust indirectly by program base";
                    case R_X86_64_RELATIVE64:
                        return "64 bit adjust by program base";
                    case R_X86_64_GOTPCRELX:
                        return "Load from 32 bit signed pc relative offset to GOT entry without REX prefix, relaxable";
                    case R_X86_64_REX_GOTPCRELX:
                        return "Load from 32 bit signed pc relative offset to GOT entry with REX prefix, relaxable";
                    default:
                        return "unknown";
                }
            }());
            names.append(QString::number(ELF64_R_SYM(rela.r_info)));

            new QTreeWidgetItem(symbolTable, names);
        }
    }
}

void treeinfo::showProgramTable() {
    ui->treeWidget->setColumnCount(static_cast<int>(programHeaderColumn.count()));
    ui->treeWidget->setHeaderLabels(programHeaderColumn);

    for (const auto &header: programHeaders) {
        QStringList names;
        names.append([&] {
            switch (header.p_type) {
                case PT_NULL:
                    return "Table entry";
                case PT_LOAD:
                    return "Loadable";
                case PT_DYNAMIC:
                    return "Dynamic linking information";
                case PT_INTERP:
                    return "Interpreter";
                case PT_NOTE:
                    return "Auxiliary information";
                case PT_SHLIB:
                    return "Reserved";
                case PT_PHDR:
                    return "Entry for header Table";
                case PT_TLS:
                    return "Thread-local storage";
                default:
                    if (header.p_type >= PT_LOOS && header.p_type <= PT_HIOS) {
                        switch (header.p_type) {
                            case PT_GNU_EH_FRAME:
                                return "OS specific GCC .eh_frame_hdr";
                            case PT_GNU_STACK:
                                return "OS specific stack executability";
                            case PT_GNU_RELRO:
                                return "OS specific read-only relocation";
                            case PT_GNU_PROPERTY:
                                return "OS specific GNU property";
                            case PT_GNU_SFRAME:
                                return "OS specific SFrame";
                            default:
                                return "OS specific";
                        }
                    }
                    if (header.p_type >= PT_LOPROC && header.p_type <= PT_HIPROC) {
                        return "Processor specific";
                    }
                    return "unknown";
            }
        }());
        QString flags;
        if (header.p_flags & PF_X)
            flags += "Executable\n";
        if (header.p_flags & PF_W)
            flags += "Writable\n";
        if (header.p_flags & PF_R)
            flags += "Readable\n";
        if (header.p_flags & PF_MASKOS)
            flags += "OS specific\n";
        if (header.p_flags & PF_MASKPROC)
            flags += "Processor specific\n";
        names.append(flags);
        names.append(QString::number(header.p_offset));
        names.append(QString::number(header.p_vaddr));
        names.append(QString::number(header.p_paddr));
        names.append(QString::number(header.p_filesz));
        names.append(QString::number(header.p_memsz));
        names.append(QString::number(header.p_align));

        auto item = new QTreeWidgetItem(ui->treeWidget, names);
        for (const auto &[sName,section]: sections) {
            if (SHT_NOBITS != section.sh_type && section.sh_offset >= header.p_offset
                && (section.sh_offset + section.sh_size) <= header.p_offset + header.p_filesz) {
                new QTreeWidgetItem(item, QStringList{sName});
            } else if (SHT_NOBITS == section.sh_type && (section.sh_addr >= header.p_vaddr
                                                         && (section.sh_addr + section.sh_size) <= (
                                                             header.p_vaddr + header.p_memsz))) {
                new QTreeWidgetItem(item, QStringList{sName});
            }
        }
    }
}
