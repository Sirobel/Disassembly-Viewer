//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_WINDOWHANDLER_H
#define DISASSEMBLY_VIEWER_WINDOWHANDLER_H

#include <QMainWindow>
#include <QStackedWidget>

#include "startwindow/recentfiles.h"
#include "textviewer/textviewer.h"
#include "settings/mainsettings.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class windowhandler;
}

QT_END_NAMESPACE

class windowhandler : public QMainWindow {
    Q_OBJECT

public:
    explicit windowhandler(QWidget *parent = nullptr);

    ~windowhandler() override;

private:
    Ui::windowhandler *ui;
    QStackedWidget *stack;
    textviewer *textViewer;
    recentfiles *recentFiles;
    mainsettings *settings = nullptr;

    void setupTextViewerMenubar();

    void setupRecentFilesMenubar();

    void updateMenubar(int index);

    void openRecentFile(const QListWidgetItem *item);

    void openFile();
};


#endif //DISASSEMBLY_VIEWER_WINDOWHANDLER_H
