//
// Created by finns on 08.04.26.
//

#ifndef DISASSEMBLY_VIEWER_FILEINFO_H
#define DISASSEMBLY_VIEWER_FILEINFO_H

#include <QWidget>
#include <textviewer/infos/columinfo.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class fileinfo;
}

QT_END_NAMESPACE

class fileinfo : public QWidget {
    Q_OBJECT

public:
    explicit fileinfo(QWidget *parent = nullptr);

    ~fileinfo() override;

    void changeWidget(int index);

    void setSectionNames(const std::vector<std::string> &names);

private:
    Ui::fileinfo *ui;
    columinfo *columInfo;

    QVector<QString> sectionNames;
};


#endif //DISASSEMBLY_VIEWER_FILEINFO_H
