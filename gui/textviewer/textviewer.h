//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_TEXTVIEWER_H
#define DISASSEMBLY_VIEWER_TEXTVIEWER_H

#include <QWidget>
#include <ElfHandler.h>
#include <QSettings>
#include <QShortcut>

#include "tree/DisasmDelegate.h"
#include "tree/DisasmModel.h"
#include "infos/fileinfo.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class textviewer;
}

QT_END_NAMESPACE

class textviewer : public QWidget {
    Q_OBJECT

public:
    explicit textviewer(QWidget *parent = nullptr);

    void openFile(const QString &filePath);

    void refresh();

    void showFileInfo(int index);


    ~textviewer() override;

private slots:
    void on_searchLineEdit_textChanged(const QString &text);

    void on_previousSearchPushButton_clicked();

    void on_nextSearchPushButton_clicked();

    void on_closeSearchButton_clicked();

private:
    Ui::textviewer *ui;
    QSettings settings;
    std::unique_ptr<ElfHandler> elf;
    std::vector<QString> sectionHeaders;
    QShortcut *shortcut;
    int totalMatches = 0;
    int currentMatch = 0;
    QTimer *searchTimer;
    DisasmModel *model;
    QModelIndex currentSearchIndex;
    DisasmDelegate *delegate;

    fileinfo *fileInfo;

    void updateSearchLabel();

    void search();

    void jumpToTarget(const QString &target);

    void handleLink(const QString &link);

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

    void toggleSearchbar();
};


#endif //DISASSEMBLY_VIEWER_TEXTVIEWER_H
