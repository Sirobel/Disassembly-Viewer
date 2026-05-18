//
// Created by finns on 08.04.26.
//

#ifndef DISASSEMBLY_VIEWER_COLUMINFO_H
#define DISASSEMBLY_VIEWER_COLUMINFO_H

#include <QWidget>
#include <elf.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class columinfo;
}

QT_END_NAMESPACE

class columinfo : public QWidget {
    Q_OBJECT

public:
    explicit columinfo(QWidget *parent = nullptr);

    void setDisplayInfo(int index);

    void setSection(const QVector<QPair<QString, Elf64_Shdr> > &data);

    void setElfHeader(const Elf64_Ehdr &header);

    QList<QString> createElfHeaderValueList();

    ~columinfo() override;

private:
    void showHeaderInfo();

    void showSectionInfo();

    Ui::columinfo *ui;

    QVector<QString> fileHeaderColumn = {"Field", "Value"};
    QVector<QString> fileHeaderRow = {
        "Magic Number", "File class", "Data encoding", "File version", "OS ABI", "ABI Version", "File type",
        "architecture", "Object file version", "Entrypoint", "Program header offset", "Section header offset",
        "processor flags",
        "ELF header size", "PHeader table size", "PHeader entries", "SHeader table size", "SHeader entries",
        "string table index"
    };
    Elf64_Ehdr fileHeader{};


    QVector<QString> sectionHeaderColumn = {
        "Name", "Type", "Flags", "Virtual address", "File offset", "Section size", "Link", "Additional section info",
        "Section alignment", "Entry size"
    };
    QVector<QPair<QString, Elf64_Shdr> > sectionHeaders;
};


#endif //DISASSEMBLY_VIEWER_COLUMINFO_H
