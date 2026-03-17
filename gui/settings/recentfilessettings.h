//
// Created by finns on 16.03.26.
//

#ifndef DISASSEMBLY_VIEWER_RECENTFILESSETTINGS_H
#define DISASSEMBLY_VIEWER_RECENTFILESSETTINGS_H

#include <QSettings>
#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class recentfilessettings;
}

QT_END_NAMESPACE

class recentfilessettings : public QWidget {
    Q_OBJECT



public:
    explicit recentfilessettings(QWidget *parent = nullptr);

    void saveSettings();

    void loadSettings();

    int getRecentFilesAmount();


    ~recentfilessettings() override;

private:
    Ui::recentfilessettings *ui;
    QSettings settings;
};


#endif //DISASSEMBLY_VIEWER_RECENTFILESSETTINGS_H
