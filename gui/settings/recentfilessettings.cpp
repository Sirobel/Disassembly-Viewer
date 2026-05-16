//
// Created by finns on 16.03.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_recentfilessettings.h" resolved

#include "recentfilessettings.h"

#include <QSettings>

#include "ui_recentfilessettings.h"


recentfilessettings::recentfilessettings(QWidget *parent) : QWidget(parent), ui(new Ui::recentfilessettings),
                                                            settings("Sirobel", "Disassembly-Viewer") {
    ui->setupUi(this);
}

void recentfilessettings::saveSettings() {
    settings.setValue("amountOfRecentFiles", ui->spinBox->value());
}

void recentfilessettings::loadSettings() {
    ui->spinBox->setValue(settings.value("amountOfRecentFiles", 10).toInt());
}

int recentfilessettings::getRecentFilesAmount() {
    return ui->spinBox->value();
}

recentfilessettings::~recentfilessettings() {
    delete ui;
}
