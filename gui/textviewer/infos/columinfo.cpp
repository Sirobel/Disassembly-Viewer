//
// Created by finns on 08.04.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_columinfo.h" resolved

#include "columinfo.h"
#include "ui_columinfo.h"


columinfo::columinfo(QWidget *parent) : QWidget(parent), ui(new Ui::columinfo) {
    ui->setupUi(this);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void columinfo::setDisplayInfo(const int index) {
    ui->tableWidget->clear();

    switch (index) {
        case 0:
            showHeaderInfo();
            break;
        case 1:
            showSectionInfo();
            break;
        default:
            break;
    }
}

void columinfo::setSection(const QVector<QPair<QString, Elf64_Shdr> > &data) {
    sectionHeaders.clear();
    sectionHeaders = data;
}

void columinfo::setElfHeader(const Elf64_Ehdr &header) {
    fileHeader = header;
}

QList<QString> columinfo::createElfHeaderValueList() {
    QList<QString> erg;

    erg.append("0x7fELF");
    erg.append([&] {
        switch (fileHeader.e_ident[EI_CLASS]) {
            case ELFCLASSNONE:
                return "invalid class";
            case ELFCLASS32:
                return "32bit object";
            case ELFCLASS64:
                return "64bit object";
            default:
                return "unknown";
        }
    }());
    erg.append([&] {
        switch (fileHeader.e_ident[EI_DATA]) {
            case ELFDATANONE:
                return "invalid data encoding";
            case ELFDATA2LSB:
                return "2's complement, little endian";
            case ELFDATA2MSB:
                return "2's complement, big endian";
            default:
                return "unknown";
        }
    }());
    erg.append(QString::number(fileHeader.e_ident[EI_VERSION]));
    erg.append([&] {
        switch (fileHeader.e_ident[EI_OSABI]) {
            case ELFOSABI_NONE:
                return "UNIX System V ABI";
            case ELFOSABI_HPUX:
                return "HP-UX";
            case ELFOSABI_NETBSD:
                return "NetBSD";
            case ELFOSABI_GNU:
                return "Object uses GNU ELF extensions";
            case ELFOSABI_SOLARIS:
                return "Sun Solaris";
            case ELFOSABI_AIX:
                return "IBM AIX";
            case ELFOSABI_IRIX:
                return "SGI Irix";
            case ELFOSABI_FREEBSD:
                return "FreeBSD";
            case ELFOSABI_TRU64:
                return "Compaq TRU64 UNIX";
            case ELFOSABI_MODESTO:
                return "Novell Modesto";
            case ELFOSABI_OPENBSD:
                return "OpenBSD";
            case ELFOSABI_ARM_AEABI:
                return "ARM EABI";
            case ELFOSABI_ARM:
                return "ARM";
            case ELFOSABI_STANDALONE:
                return "standalone";
            default:
                return "unknown";
        }
    }());
    erg.append(QString::number(fileHeader.e_ident[EI_ABIVERSION]));
    erg.append([&] {
        switch (fileHeader.e_type) {
            case ET_NONE:
                return "no file type";
            case ET_REL:
                return "Relocatable file";
            case ET_EXEC:
                return "Executable file";
            case ET_DYN:
                return "Shared object file";
            case ET_CORE:
                return "Core file";
            case ET_LOOS:
                return "OS-specific range start";
            case ET_HIOS:
                return "OS-specific range end";
            case ET_LOPROC:
                return "Processor-specific range start";
            case ET_HIPROC:
                return "Processor-specific range end";
            default:
                return "unknown";
        }
    }());
    erg.append("AMD x86-64 architecture");
    erg.append(QString::number(fileHeader.e_version));
    erg.append("0x" + QString::number(fileHeader.e_entry, 16));
    erg.append("0x" + QString::number(fileHeader.e_phoff, 16));
    erg.append("0x" + QString::number(fileHeader.e_shoff, 16));
    erg.append("0x" + QString::number(fileHeader.e_flags, 16));
    erg.append("0x" + QString::number(fileHeader.e_ehsize, 16));
    erg.append("0x" + QString::number(fileHeader.e_phentsize, 16));
    erg.append("0x" + QString::number(fileHeader.e_phnum, 16));
    erg.append("0x" + QString::number(fileHeader.e_shentsize, 16));
    erg.append("0x" + QString::number(fileHeader.e_shnum, 16));
    erg.append("0x" + QString::number(fileHeader.e_shstrndx, 16));

    return erg;
}

void columinfo::showHeaderInfo() {
    ui->tableWidget->setColumnCount(static_cast<int>(fileHeaderColumn.size()));
    ui->tableWidget->setRowCount(static_cast<int>(fileHeaderRow.size()));

    ui->tableWidget->setHorizontalHeaderLabels(fileHeaderColumn);


    auto values = createElfHeaderValueList();

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(fileHeaderRow[i]));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(values[i]));
    }
    update();
}

static constexpr std::pair<uint64_t, const char *> flagNames[] = {
    {SHF_WRITE, "Writable"},
    {SHF_ALLOC, "Allocated during execution"},
    {SHF_EXECINSTR, "Executable"},
    {SHF_MERGE, "Might be merged"},
    {SHF_STRINGS, "Contains nul-terminated strings"},
    {SHF_INFO_LINK, "Info contains SHT index"},
    {SHF_LINK_ORDER, "Preserve order after combining"},
    {SHF_OS_NONCONFORMING, "Non standard OS specific handling"},
    {SHF_GROUP, "Member of a group"},
    {SHF_TLS, "Thread local data"},
    {SHF_COMPRESSED, "Compressed"},
    {SHF_MASKOS, "OS specific"},
    {SHF_MASKPROC, "Processor specific"},
    {SHF_GNU_RETAIN, "Not to be GCed by linker"},
    {SHF_ORDERED, "Specific ordering required"},
    {SHF_EXCLUDE, "Section is excluded unless referenced or allocated"},
};


void columinfo::showSectionInfo() {
    ui->tableWidget->setColumnCount(static_cast<int>(sectionHeaderColumn.size()));
    ui->tableWidget->setRowCount(static_cast<int>(sectionHeaders.size()));
    ui->tableWidget->setHorizontalHeaderLabels(sectionHeaderColumn);

    QVector<QStringList> rows;
    rows.reserve(sectionHeaders.size());
    for (const auto &[name, header]: sectionHeaders) {
        QStringList list;
        list.append(name);
        list.append([header] {
            switch (header.sh_type) {
                case SHT_NULL:
                    return "section Header table entry";
                case SHT_PROGBITS:
                    return "Program data";
                case SHT_SYMTAB:
                    return "Symbol table";
                case SHT_STRTAB:
                    return "string table";
                case SHT_RELA:
                    return "Relocation table with addends";
                case SHT_HASH:
                    return "Symbol hash table";
                case SHT_DYNAMIC:
                    return "Dynamic linking information";
                case SHT_NOTE:
                    return "Notes";
                case SHT_NOBITS:
                    return "Program space with no data";
                case SHT_REL:
                    return "relocation table with no addends";
                case SHT_SHLIB:
                    return "reserved";
                case SHT_DYNSYM:
                    return "Dynamic linker symbol table";
                case SHT_INIT_ARRAY:
                    return "Array of constructors";
                case SHT_FINI_ARRAY:
                    return "Array of destructors";
                case SHT_PREINIT_ARRAY:
                    return "Array of pre-constructors";
                case SHT_GROUP:
                    return "Section group";
                case SHT_SYMTAB_SHNDX:
                    return "Extended section indices";
                case SHT_RELR:
                    return "RELR relative relocations";
                default:
                    if (header.sh_type >= SHT_LOOS && header.sh_type <= SHT_HIOS)
                        return "OS-specific";
                    if (header.sh_type >= SHT_LOPROC && header.sh_type <= SHT_HIPROC)
                        return "Processor specific";
                    if (header.sh_type >= SHT_LOUSER && header.sh_type <= SHT_HIUSER)
                        return "Application specific";
                    return "unknown";
            }
        }());
        QString flag;
        for (const auto &[bits, FlagName]: flagNames)
            if (header.sh_flags & bits)
                flag += QString("%1\n").arg(FlagName);

        list.append(flag);
        list.append(QString::number(header.sh_addr));
        list.append(QString::number(header.sh_offset));
        list.append(QString::number(header.sh_size));
        list.append(QString::number(header.sh_link));
        list.append(QString::number(header.sh_info));
        list.append(QString::number(header.sh_addralign));
        list.append(QString::number(header.sh_entsize));

        rows.append(list);
    }

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        for (int j = 0; j < sectionHeaderColumn.size(); j++) {
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(rows[i][j]));
        }
    }
    update();
}

columinfo::~columinfo() {
    delete ui;
}
