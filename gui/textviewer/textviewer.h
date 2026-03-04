//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_TEXTVIEWER_H
#define DISASSEMBLY_VIEWER_TEXTVIEWER_H

#include <QWidget>
#include <ElfHandler.h>


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

    ~textviewer() override;

private slots:
    void on_textBrowser_anchorClicked(const QUrl &arg1);

private:
    Ui::textviewer *ui;
    std::unique_ptr<ElfHandler> elf;
    std::vector<QString> sectionHeaders;
};


#endif //DISASSEMBLY_VIEWER_TEXTVIEWER_H
