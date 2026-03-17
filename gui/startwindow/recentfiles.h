//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_RECENTFILES_H
#define DISASSEMBLY_VIEWER_RECENTFILES_H

#include <QListWidget>
#include <QSettings>
#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class recentfiles;
}

QT_END_NAMESPACE

class recentfiles : public QWidget {
    Q_OBJECT

public:
    explicit recentfiles(QWidget *parent = nullptr);

    ~recentfiles() override;

    QListWidget *getListWidget();

    void addFiletoList(const QString &filePath);

    void setAmount(int amount);

    void refresh();

private:
    Ui::recentfiles *ui;
    int listAmount;
    QSettings settings;
};


#endif //DISASSEMBLY_VIEWER_RECENTFILES_H
