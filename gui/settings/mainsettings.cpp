//
// Created by finns on 16.03.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mainsettings.h" resolved

#include "mainsettings.h"
#include "ui_mainsettings.h"


mainsettings::mainsettings(QWidget *parent) : QWidget(parent), ui(new Ui::mainsettings) {
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    ui->splitter->setSizes({150, 600});

    recentFilesSettings = new recentfilessettings(this);
    textViewerSettings = new textviewersettings(this);

    ui->stackedWidget->addWidget(recentFilesSettings);
    ui->stackedWidget->addWidget(textViewerSettings);

    recentFilesSettings->loadSettings();
    textViewerSettings->loadSettings();


    for (auto &names: settingSections) {
        ui->listWidget->addItem(names);
    }

    ui->stackedWidget->setCurrentIndex(0);
}

mainsettings::~mainsettings() {
    delete ui;
}


void mainsettings::on_listWidget_itemClicked(QListWidgetItem *item) {
    const auto name = item->text();

    for (int i = 0; i < settingSections.size(); ++i) {
        if (name == settingSections[i]) {
            ui->stackedWidget->setCurrentIndex(i);
        }
    }
}

void mainsettings::on_CancelPushButton_clicked() {
    recentFilesSettings->loadSettings();
    textViewerSettings->loadSettings();
    close();
}

void mainsettings::on_OkPushButton_clicked() {
    on_ApplyPushButton_clicked();
    close();
}


void mainsettings::on_ApplyPushButton_clicked() {
    recentFilesSettings->saveSettings();
    textViewerSettings->saveSettings();
    emit SavedSettings();
}
