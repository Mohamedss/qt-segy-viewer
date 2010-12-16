#ifndef SETBYTE_H
#define SETBYTE_H

#include <QDialog>
#include <QtGui>

namespace Ui {
    class setbyte;
}

class setbyte : public QDialog
{
    Q_OBJECT

public:
    explicit setbyte(QWidget *parent = 0);
    ~setbyte();
    int byteCDP;
    int byteSP;
    int byteX;
    int byteY;
    int byteInline;
    int byteXline;
    int byteLineName;
    int byteLineNameLength;
    void InitByte(int,int,int,int,int,int,int,int);

private:
    Ui::setbyte *ui;

private slots:
    void on_buttonBox_accepted();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem*, int);
};

#endif // SETBYTE_H
