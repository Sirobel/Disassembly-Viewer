//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_recentfiles.h" resolved

#include "recentfiles.h"
#include "ui_recentfiles.h"
#include <QSettings>


recentfiles::recentfiles(QWidget *parent) : QWidget(parent), ui(new Ui::recentfiles) {
    ui->setupUi(this);

    const QSettings settings("Sirobel", "Disassembly-Viewer");

    for (auto filepaths = settings.value("recentFiles").toStringList(); const QString &filePath: filepaths) {
        ui->PathListWidget->addItem(new QListWidgetItem(filePath));
    }
}

recentfiles::~recentfiles() {
    delete ui;
}

QListWidget *recentfiles::getListWidget() {
    return ui->PathListWidget;
}

void recentfiles::addFiletoList(const QString &filePath) {
    for (QList<QListWidgetItem *> dups = ui->PathListWidget->findItems(filePath, Qt::MatchExactly); const auto *i: dups)
        delete i;

    ui->PathListWidget->insertItem(0, filePath);

    while (ui->PathListWidget->count() > 10) {
        delete ui->PathListWidget->takeItem(ui->PathListWidget->count() - 1);
    }

    QStringList list;
    list.reserve(ui->PathListWidget->count());
    for (int i = 0; i < ui->PathListWidget->count(); ++i) { list << ui->PathListWidget->item(i)->text(); }
    QSettings settings("Sirobel", "Disassembly-Viewer");
    settings.setValue("recentFiles", list);
}
