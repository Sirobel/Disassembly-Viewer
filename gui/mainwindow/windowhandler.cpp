//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_windowhandler.h" resolved

#include "windowhandler.h"

#include <iostream>
#include <QListWidget>
#include <QFileDialog>

#include "ui_windowhandler.h"


windowhandler::windowhandler(QWidget *parent) : QMainWindow(parent), ui(new Ui::windowhandler) {
    ui->setupUi(this);
    stack = new QStackedWidget(this);

    textViewer = new textviewer(this);
    recentFiles = new recentfiles(this);

    stack->addWidget(recentFiles);
    stack->addWidget(textViewer);

    setCentralWidget(stack);

    connect(stack, &QStackedWidget::currentChanged, this, &windowhandler::updateMenubar);
    stack->setCurrentWidget(recentFiles);
    updateMenubar(0);

    connect(recentFiles->getListWidget(), &QListWidget::itemClicked, this, &windowhandler::openRecentFile);

    stack->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(stack->currentWidget()->minimumSize());
    resize(stack->currentWidget()->sizeHint());
}

windowhandler::~windowhandler() {
    delete ui;
}

void windowhandler::setupTextViewerMenubar() {
    menuBar()->clear();
    const auto fileMenu = menuBar()->addMenu("File");

    const auto openAction = fileMenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, &windowhandler::openFile);

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

void windowhandler::openRecentFile(QListWidgetItem *item) {
    QString path = item->text();
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
