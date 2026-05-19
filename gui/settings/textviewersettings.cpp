//
// Created by finns on 16.03.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_textviewersettings.h" resolved

#include "textviewersettings.h"

#include <QPainter>

#include "ui_textviewersettings.h"


textviewersettings::textviewersettings(QWidget *parent) : QWidget(parent), ui(new Ui::textviewersettings),
                                                          settings("Sirobel", "Disassembly-Viewer") {
    ui->setupUi(this);
}

void textviewersettings::loadSettings() {
    ui->linkColorLineEdit->setText(settings.value("linkColor","#0000FF").toString());
    ui->textColorLineEdit->setText(settings.value("textColor","#000000").toString());
    ui->linkUnsercoreCheckBox->setCheckState(
        static_cast<Qt::CheckState>(settings.value("linkUnderscore", Qt::Checked).toInt())
    );
    ui->fontSizeSpinBox->setValue(settings.value("fontSize",12).toInt());
    ui->sectionListWidget->addItems(
        settings.value("sections", QStringList{".init", ".plt", ".plt.got", ".plt.sec", ".text", ".fini"}).
        toStringList());

    addColorsToMemBarList(settings.value("memBarColors",QStringList{"#5B2D8E","#9B4DCA","#9B4DCA","#1F4068","#2E6B5E","#C0392B"}).toStringList());
    ui->memBarTextColorLineEdit->setText(settings.value("memBarTextColor","#ffffff").toString());
    ui->memBarBorderColorLineEdit->setText(settings.value("memBarBorderColor","#000000").toString());
    ui->memBarFontSizeSpinBox->setValue(settings.value("memBarFontSize",10).toInt());
    ui->memBarBorderSizeSpinBox->setValue(settings.value("memBarBorderSize",1).toInt());
}

void textviewersettings::saveSettings() {
    settings.setValue("linkColor", ui->linkColorLineEdit->text());
    settings.setValue("textColor", ui->textColorLineEdit->text());
    settings.setValue("linkUnderscore", ui->linkUnsercoreCheckBox->checkState());
    settings.setValue("fontSize", ui->fontSizeSpinBox->value());

    QStringList sections;
    sections.reserve(ui->sectionListWidget->count());
    for (int i = 0; i < ui->sectionListWidget->count(); ++i) {
        sections << ui->sectionListWidget->item(i)->text();
    }
    settings.setValue("sections", sections);

    QStringList list;
    list.reserve(ui->colorListWidget->count());
    for (int i = 0; i < ui->colorListWidget->count(); ++i) { list << ui->colorListWidget->item(i)->text(); }
    settings.setValue("memBarColors", list);

    settings.setValue("memBarTextColor", ui->memBarTextColorLineEdit->text());
    settings.setValue("memBarBorderColor", ui->memBarBorderColorLineEdit->text());
    settings.setValue("memBarFontSize", ui->memBarFontSizeSpinBox->value());
    settings.setValue("memBarBorderSize", ui->memBarBorderSizeSpinBox->value());
}

textviewersettings::~textviewersettings() {
    delete ui;
}


void textviewersettings::on_linkColorLineEdit_textChanged(const QString &text) {
    QColor c(text);

    if (c.isValid()) {
        ui->linkColorDisplay->setStyleSheet(
            "background: " + text + "; border: 1px solid black;");
    } else {
        ui->linkColorDisplay->setStyleSheet(
            "background: transparent;"
        );
    }
}


void textviewersettings::on_textColorLineEdit_textChanged(const QString &text) {
    QColor c(text);

    if (c.isValid()) {
        ui->textColorDisplay->setStyleSheet(
            "background: " + text + "; border: 1px solid black;");
    } else {
        ui->textColorDisplay->setStyleSheet(
            "background: transparent;"
        );
    }
}

void textviewersettings::on_addColorPushButton_clicked() {
    QColor c(ui->colorListLineEdit->text());
    if (!c.isValid()) {
        return;
    }

    QListWidgetItem item(ui->colorListLineEdit->text());
    QPixmap pixmap(20, 20);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.fillRect(0, 0, 20, 20, c);
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, 19, 19);

    painter.end();

    item.setIcon(QIcon(pixmap));
    ui->colorListWidget->addItem(new QListWidgetItem(item));
}

void textviewersettings::on_colorListWidget_itemClicked(QListWidgetItem *item) {
    ui->deletePushButton->setDisabled(false);
}

void textviewersettings::on_deletePushButton_clicked() {
    for (auto selected = ui->colorListWidget->selectedItems(); const auto &item: selected) {
        delete ui->colorListWidget->takeItem(ui->colorListWidget->row(item));
    }
    if (ui->colorListWidget->selectedItems().isEmpty())
        ui->deletePushButton->setDisabled(true);
}

void textviewersettings::on_memBarTextColorLineEdit_textChanged(const QString &text) {
    QColor c(text);

    if (c.isValid()) {
        ui->memBarTextColorDisplay->setStyleSheet(
            "background: " + text + "; border: 1px solid black;");
    } else {
        ui->memBarTextColorDisplay->setStyleSheet(
            "background: transparent;"
        );
    }
}

void textviewersettings::on_memBarBorderColorLineEdit_textChanged(const QString &text) {
    QColor c(text);

    if (c.isValid()) {
        ui->memBarBorderColorDisplay->setStyleSheet(
            "background: " + text + "; border: 1px solid black;");
    } else {
        ui->memBarBorderColorDisplay->setStyleSheet(
            "background: transparent;"
        );
    }
}

void textviewersettings::addColorsToMemBarList(const QStringList &items) {
    for (const auto &text: items) {
        QColor c(text);
        if (!c.isValid()) {
            return;
        }

        QListWidgetItem item(text);
        QPixmap pixmap(20, 20);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);

        painter.fillRect(0, 0, 20, 20, c);
        painter.setPen(Qt::black);
        painter.drawRect(0, 0, 19, 19);

        painter.end();

        item.setIcon(QIcon(pixmap));
        ui->colorListWidget->addItem(new QListWidgetItem(item));
    }
}

void textviewersettings::on_sectionAddPushButton_clicked() {
    if (ui->sectionLineEdit->text().isEmpty())
        return;

    ui->sectionListWidget->addItem(new QListWidgetItem(ui->sectionLineEdit->text()));
}


void textviewersettings::on_sectionDeletePushButton_clicked() {
    for (auto selected = ui->sectionListWidget->selectedItems(); const auto &item: selected) {
        delete ui->sectionListWidget->takeItem(ui->sectionListWidget->row(item));
    }
    if (ui->sectionListWidget->selectedItems().isEmpty())
        ui->deletePushButton->setDisabled(true);
}
