//
// Created by finns on 16.03.26.
//

#ifndef DISASSEMBLY_VIEWER_MAINSETTINGS_H
#define DISASSEMBLY_VIEWER_MAINSETTINGS_H

#include <QWidget>

#include "recentfilessettings.h"
#include "textviewersettings.h"
#include <QListWidgetItem>


QT_BEGIN_NAMESPACE

namespace Ui {
    class mainsettings;
}

QT_END_NAMESPACE

class mainsettings : public QWidget {
    Q_OBJECT
signals:
    void RecentFilesAmount(int amount);

public:
    explicit mainsettings(QWidget *parent = nullptr);

    ~mainsettings() override;

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_CancelPushButton_clicked();

    void on_OkPushButton_clicked();

    void on_ApplyPushButton_clicked();

private:
    Ui::mainsettings *ui;

    QList<QString> settingSections = {"RecentFiles", "TextViewer"};
    recentfilessettings *recentFilesSettings;
    textviewersettings *textViewerSettings;
};


#endif //DISASSEMBLY_VIEWER_MAINSETTINGS_H
