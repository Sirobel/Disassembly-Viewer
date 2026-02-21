//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_recentfiles.h" resolved

#include "recentfiles.h"
#include "ui_recentfiles.h"


recentfiles::recentfiles(QWidget *parent) : QWidget(parent), ui(new Ui::recentfiles) {
    ui->setupUi(this);

    std::vector<QString> filePaths;
    filePaths.emplace_back("test1");
    filePaths.emplace_back("test2");
    filePaths.emplace_back("test3");

    for (const QString& filePath: filePaths) {
        ui->PathListWidget->addItem(new QListWidgetItem(filePath));
    }
}

recentfiles::~recentfiles() {
    delete ui;
}

QListWidget * recentfiles::getListWidget() {
    return ui->PathListWidget;
}

void recentfiles::addFiletoList(QString filePath) {
    for (QList<QListWidgetItem *> dups = ui->PathListWidget->findItems(filePath,Qt::MatchExactly); const auto *i: dups)
        delete i;

    ui->PathListWidget->insertItem(0, filePath);

    while (ui->PathListWidget->count()>10) {
        delete ui->PathListWidget->takeItem(ui->PathListWidget->count()-1);
    }

}
