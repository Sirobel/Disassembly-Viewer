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

    void setSectionNames(const QVector<QString> &names);
    void setElfHeader(const Elf64_Ehdr &header);

    ~columinfo() override;

private:
    void showHeaderInfo();

    void showSectionInfo();

    Ui::columinfo *ui;

    QVector<QString> fileHeaderColum = {"Field", "Value"};
    QVector<QString> fileHeaderRow = {
        "Magic Number", "Object type", "Architecture", "File version", "Entrypoint", "Program header offset",
        "Section header offset", "processor flags", "ELF header size", "PHeader table size", "PHeader entries",
        "SHeader table size", "SHeader entries", "string table index"
    };
    Elf64_Ehdr fileHeader;


    QVector<QString> sectionHeaderColum = {"Name"};
    QVector<QString> sectionHeaderRow = {};
};


#endif //DISASSEMBLY_VIEWER_COLUMINFO_H
