//
// Created by finns on 16.03.26.
//

#ifndef DISASSEMBLY_VIEWER_TEXTVIEWERSETTINGS_H
#define DISASSEMBLY_VIEWER_TEXTVIEWERSETTINGS_H

#include <QListWidgetItem>
#include <QSettings>
#include <QWidget>
#include <QList>


QT_BEGIN_NAMESPACE

namespace Ui {
    class textviewersettings;
}

QT_END_NAMESPACE

class textviewersettings : public QWidget {
    Q_OBJECT

public:
    explicit textviewersettings(QWidget *parent = nullptr);

    void loadSettings();

    void saveSettings();

    ~textviewersettings() override;

private slots:
    void on_linkColorLineEdit_textChanged(const QString &text);

    void on_textColorLineEdit_textChanged(const QString &text);

    void on_addColorPushButton_clicked();

    void on_colorListWidget_itemClicked(QListWidgetItem *item);

    void on_deletePushButton_clicked();

    void on_memBarTextColorLineEdit_textChanged(const QString &text);

    void on_memBarBorderColorLineEdit_textChanged(const QString &text);

private:
    Ui::textviewersettings *ui;
    QSettings settings;

    void addColorsToMemBarList(const QStringList &items);
};


#endif //DISASSEMBLY_VIEWER_TEXTVIEWERSETTINGS_H
