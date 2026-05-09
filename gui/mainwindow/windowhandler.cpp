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

    settings = new mainsettings(this);
    connect(settings, &mainsettings::SavedSettings, this, [this]() {
        recentFiles->refresh();
        textViewer->refresh();
    });
    settings->close();

    connect(recentFiles->getListWidget(), &QListWidget::itemClicked, this, [this](const QListWidgetItem *item) {
        openRecentFile(item->text());
    });
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
        settings->setWindowFlag(Qt::Window);
        settings->setWindowModality(Qt::ApplicationModal);
        settings->show();
        settings->raise();
        settings->activateWindow();
    });

    const auto closeAction = fileMenu->addAction("Close");
    connect(closeAction, &QAction::triggered, [this]() {
        stack->setCurrentWidget(recentFiles);
    });

    const auto infoMenu = menuBar()->addMenu("Info");
    const auto infoAction = infoMenu->addAction("Info");
    connect(infoAction, &QAction::triggered, [this]() {
        textViewer->showFileInfo(0);
    });

    const auto sectionAction = infoMenu->addAction("Sections");
    connect(sectionAction, &QAction::triggered, [this]() {
        textViewer->showFileInfo(1);
    });

    const auto stringTableAction = infoMenu->addAction("StringTables");
    connect(stringTableAction, &QAction::triggered, [this]() {
        textViewer->showFileInfo(2);
    });

    const auto symbolTableAction = infoMenu->addAction("SymbolTables");
    connect(symbolTableAction, &QAction::triggered, [this]() {
        textViewer->showFileInfo(3);
    });

    const auto relocationTableAction = infoMenu->addAction("Relocations");
    connect(relocationTableAction, &QAction::triggered, [this]() {
        textViewer->showFileInfo(4);
    });
}

void windowhandler::setupRecentFilesMenubar() {
    menuBar()->clear();
    const auto fileMenu = menuBar()->addMenu("File");
    const auto openAction = fileMenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, &windowhandler::openFile);

    const auto settingsAction = fileMenu->addAction("Settings");
    connect(settingsAction, &QAction::triggered, [this]() {
        settings->setWindowFlag(Qt::Window);
        settings->setWindowModality(Qt::ApplicationModal);
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

void windowhandler::openRecentFile(const QString &path) {
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
