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

void treeinfo::showStringTables() {
    ui->treeWidget->setColumnCount(static_cast<int>(stringTableColumn.count()));
    ui->treeWidget->setHeaderLabels(stringTableColumn);

    for (const auto &[section, strings]: stringTables.asKeyValueRange()) {
        auto item = std::make_unique<QTreeWidgetItem>(QStringList{section});

        for (const auto &string: strings) {
            auto child = std::make_unique<QTreeWidgetItem>(QStringList{string});
            item->addChild(child.get());
            [[maybe_unused]] auto b = child.release();
        }

        ui->treeWidget->addTopLevelItem(item.get());
        [[maybe_unused]] auto a = item.release();
    }
}

void treeinfo::showSymbolTables() {
    ui->treeWidget->setColumnCount(static_cast<int>(symbolTableColumn.count()));
    ui->treeWidget->setHeaderLabels(symbolTableColumn);

    for (const auto &[section, strings]: symbolTables.asKeyValueRange()) {
        auto item = std::make_unique<QTreeWidgetItem>(QStringList{section});
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
                    case STT_NUM:
                        return "number";
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
                    case STB_NUM:
                        return "number";
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


            auto child = std::make_unique<QTreeWidgetItem>(names);
            item->addChild(child.get());
            [[maybe_unused]] auto b = child.release();
        }

        ui->treeWidget->addTopLevelItem(item.get());
        [[maybe_unused]] auto a = item.release();
    }
}
