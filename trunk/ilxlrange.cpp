#include "ilxlrange.h"
#include "ui_ilxlrange.h"

ilxlrange::ilxlrange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ilxlrange)
{
    ui->setupUi(this);
    connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(onStart()));
}

ilxlrange::~ilxlrange()
{
    delete ui;
}

void ilxlrange::onStart()
{
    byteInline = ui->spinboxInlineByte->text().toInt();
    byteXline  = ui->spinboxXlineByte->text().toInt();
    accept();
}
