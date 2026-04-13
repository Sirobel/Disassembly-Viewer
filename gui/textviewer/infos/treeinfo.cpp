//
// Created by finns on 09.04.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_treeinfo.h" resolved

#include "treeinfo.h"
#include "ui_treeinfo.h"


treeinfo::treeinfo(QWidget *parent) : QWidget(parent), ui(new Ui::treeinfo) {
    ui->setupUi(this);
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
        for (const auto &key: strings | std::views::keys) {
            auto child = std::make_unique<QTreeWidgetItem>(QStringList{key});
            item->addChild(child.get());
            [[maybe_unused]] auto b = child.release();
        }

        ui->treeWidget->addTopLevelItem(item.get());
        [[maybe_unused]] auto a = item.release();
    }
}
