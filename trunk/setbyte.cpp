#include <QtGui>
#include "setbyte.h"
#include "ui_setbyte.h"

setbyte::setbyte(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setbyte)
{
    ui->setupUi(this);   
}

setbyte::~setbyte()
{
    delete ui;
}

void setbyte::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if(column == 1)
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
}

void setbyte::on_buttonBox_accepted()
{
    byteLineName = ui->treeWidget->topLevelItem(0)->text(1).toInt();
    byteLineNameLength = ui->treeWidget->topLevelItem(1)->text(1).toInt();
    byteCDP = ui->treeWidget->topLevelItem(2)->text(1).toInt();
    byteSP = ui->treeWidget->topLevelItem(3)->text(1).toInt();
    byteX = ui->treeWidget->topLevelItem(4)->text(1).toInt();
    byteY = ui->treeWidget->topLevelItem(5)->text(1).toInt();
    byteInline = ui->treeWidget->topLevelItem(6)->text(1).toInt();
    byteXline = ui->treeWidget->topLevelItem(7)->text(1).toInt();
}

void setbyte::InitByte(int cdp, int sp, int x, int y, int name, int length, int il, int xl)
{
    QTreeWidgetItem *item;
    item = ui->treeWidget->topLevelItem(0);
    item->setText(1,QString::number(name));
    item = ui->treeWidget->topLevelItem(1);
    item->setText(1,QString::number(length));
    item = ui->treeWidget->topLevelItem(2);
    item->setText(1,QString::number(cdp));
    item = ui->treeWidget->topLevelItem(3);
    item->setText(1,QString::number(sp));
    item = ui->treeWidget->topLevelItem(4);
    item->setText(1,QString::number(x));
    item = ui->treeWidget->topLevelItem(5);
    item->setText(1,QString::number(y));
    item = ui->treeWidget->topLevelItem(6);
    item->setText(1,QString::number(il));
    item = ui->treeWidget->topLevelItem(7);
    item->setText(1,QString::number(xl));
}
