//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_windowhandler.h" resolved

#include "windowhandler.h"

#include <iostream>
#include <QListWidget>
#include <QFileDialog>

#include "ui_windowhandler.h"
#include "settings/mainsettings.h"


windowhandler::windowhandler(QWidget *parent) : QMainWindow(parent), ui(new Ui::windowhandler) {
    ui->setupUi(this);
    stack = new QStackedWidget(this);

    textViewer = new textviewer(this);
    recentFiles = new recentfiles(this);

    stack->addWidget(recentFiles);
    stack->addWidget(textViewer);

    ui->verticalLayout_2->addWidget(stack);

    connect(stack, &QStackedWidget::currentChanged, this, &windowhandler::updateMenubar);
    stack->setCurrentWidget(recentFiles);
    updateMenubar(0);

    connect(recentFiles->getListWidget(), &QListWidget::itemClicked, this, &windowhandler::openRecentFile);
}

windowhandler::~windowhandler() {
    delete ui;
}

void windowhandler::setupTextViewerMenubar() {
    menuBar()->clear();
    const auto fileMenu = menuBar()->addMenu("File");

    const auto openAction = fileMenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, &windowhandler::openFile);

    const auto settingsAction = fileMenu->addAction("Settings");
    connect(settingsAction, &QAction::triggered, [this]() {
        if (!settings) {
            settings = new mainsettings(this);
            connect(settings, &mainsettings::SavedSettings, this, [this]() {
                recentFiles->refresh();
            });
        }

        settings->setWindowFlag(Qt::Window);
        settings->show();
        settings->raise();
        settings->activateWindow();
    });

    const auto closeAction = fileMenu->addAction("Close");
    connect(closeAction, &QAction::triggered, [this]() {
        stack->setCurrentWidget(recentFiles);
    });

    const auto infoMenu = menuBar()->addMenu("Info");
}

void windowhandler::setupRecentFilesMenubar() {
    menuBar()->clear();
    const auto fileMenu = menuBar()->addMenu("File");
    const auto openAction = fileMenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, &windowhandler::openFile);

    const auto settingsAction = fileMenu->addAction("Settings");
    connect(settingsAction, &QAction::triggered, [this]() {
        if (!settings) {
            settings = new mainsettings(this);
            connect(settings, &mainsettings::SavedSettings, this, [this]() {
                recentFiles->refresh();
            });
        }

        settings->setWindowFlag(Qt::Window);
        settings->show();
        settings->raise();
        settings->activateWindow();
    });
}

void windowhandler::updateMenubar(const int index) {
    switch (index) {
        case 0:
            setupRecentFilesMenubar();
            break;
        case 1:
            setupTextViewerMenubar();
            break;
        default:
            std::cerr << "Switched to unknown widget" << std::endl;
    }
}

void windowhandler::openRecentFile(const QListWidgetItem *item) {
    const QString path = item->text();
    recentFiles->addFiletoList(path);
    stack->setCurrentWidget(textViewer);
    textViewer->openFile(path);
}

void windowhandler::openFile() {
    const QString filename = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (filename.isEmpty())
        return;

    recentFiles->addFiletoList(filename);

    textViewer->openFile(filename);
    stack->setCurrentWidget(textViewer);
}
