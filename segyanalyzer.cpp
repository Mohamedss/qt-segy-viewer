#include <QtGui>
#include <segyanalyzer.h>
#include <ui_segyanalyzer.h>
#include <setbyte.h>
#include <printview.h>
#include <qprinter.h>

segyanalyzer::segyanalyzer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::segyanalyzer)
{
    ui->setupUi(this);

    CenteredWindow();

    SetupTable();

    segy = new SEGY();

    ui->lbByteLocation->setText("Line Name=<strong>" + QString::number(segy->_ByteLineName) + "</strong> , Line Name Length=<strong>" + QString::number(segy->_LineNameLength) + "</strong> , CDP=<strong>" + QString::number(segy->_ByteCDP) + "</strong> , SP=<strong>" + QString::number(segy->_ByteSP) + "</strong> , X=<strong>" + QString::number(segy->_ByteX) + "</strong> , Y=<strong>" + QString::number(segy->_ByteY) + "</strong> , Inline=<strong>" + QString::number(segy->_ByteInline) + "</strong> , Xline=<strong>" + QString::number(segy->_ByteXline) + "</strong>");

    isFileSelected = isByteSetted = isAnalyzed = false;

    connect(ui->btnSelectSEGYFiles, SIGNAL(clicked()), this, SLOT(SelectSEGYFiles()));
    connect(ui->btnSetByteLocation, SIGNAL(clicked()), this, SLOT(RunSetByte()));
    connect(ui->btnStartAnalyze, SIGNAL(clicked()), this, SLOT(Analyze()));
    connect(ui->btnPrint, SIGNAL(clicked()), this, SLOT(Print()));
}

segyanalyzer::~segyanalyzer()
{
    delete ui;
}

void segyanalyzer::SetupTable()
{
    ui->tableSEGY->resizeColumnsToContents();
    //ui->tableSEGY->setColumnWidth(0, 200);
    //ui->tableSEGY->setColumnWidth(1, 150);
    ui->tableSEGY->setColumnWidth(2, 80);
    ui->tableSEGY->setColumnWidth(3, 80);
    ui->tableSEGY->setColumnWidth(4, 80);
    ui->tableSEGY->setColumnWidth(5, 80);
    ui->tableSEGY->setColumnWidth(6, 80);
    ui->tableSEGY->setColumnWidth(7, 80);
    ui->tableSEGY->setColumnWidth(8, 80);
    ui->tableSEGY->setColumnWidth(9, 80);
    ui->tableSEGY->setColumnWidth(10, 80);
    ui->tableSEGY->setColumnWidth(11, 80);
    ui->tableSEGY->setColumnWidth(12, 80);
    ui->tableSEGY->setColumnWidth(13, 80);
    ui->tableSEGY->setColumnWidth(14, 80);
    ui->tableSEGY->resizeRowsToContents();
}

void segyanalyzer::CenteredWindow()
{
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth, appWidth;
    int screenHeight, appHeight;
    int x, y;
    QSize windowSize;
    screenWidth = desktop->width();
    if(screenWidth > 2000) screenWidth /= 2;
    screenHeight = desktop->height();
    windowSize = size();
    appWidth = windowSize.width();
    appHeight = windowSize.height();
    x = (screenWidth - appWidth) / 2;
    y = (screenHeight - appHeight) / 2;
    y -= 50;
    move(x, y);
}

void segyanalyzer::SelectSEGYFiles()
{
    if(ui->tableSEGY->rowCount() > 0 &&
       QMessageBox::warning(
         this, "Clear all?",
         "Clear all?",
         "&Yes", "&No", QString::null, 1, 1))
    {
        return;
    }
    else
    {
        ClearTable();
    }

    QStringList filenames = QFileDialog::getOpenFileNames(
            this,
            tr("Select SEGY Files"),
            QDir::currentPath(),
            tr("SEGY (*.sgy *.segy)")
    );
    if(!filenames.isEmpty())
    {
        QDir dir = QFileInfo(filenames.at(0)).absoluteDir();
        selectedDir = dir.absolutePath();
        ui->lbDirPath->setText("Directory: <strong>" + dir.absolutePath().replace("/","\\") + "</strong>");
        ui->lbTotalFiles->setText("Total Files: <strong>" + QString::number(filenames.size()) + "</strong>");
        foreach(QString name, filenames)
        {
            int row = ui->tableSEGY->rowCount();
            AddRow();
            QFileInfo fi(name);
            ui->tableSEGY->item(row,0)->setText(fi.fileName());
        }
        SetupTable();
        ui->btnSetByteLocation->setEnabled(true);
        ui->btnStartAnalyze->setEnabled(true);
        isAnalyzed = false;
    }
}

void segyanalyzer::ClearTable()
{
    int rowCount = ui->tableSEGY->rowCount();
    for(int i=0; i<rowCount; i++)
    {
        ui->tableSEGY->removeRow(0);
    }
}

void segyanalyzer::Analyze()
{
    if(isAnalyzed && QMessageBox::warning(this, "Analyze again?", "Analyze again?   ", "Yes", "No"))
    {
       return;
    }
    for(int row=0; row<ui->tableSEGY->rowCount(); row++)
    {
        QString file(selectedDir);
        file.append("/"); file.append(ui->tableSEGY->item(row,0)->text());

        segy->OpenFile(file.toAscii().data());
        segy->ReadFirstAndLast();

        ui->tableSEGY->item(row,1)->setText(segy->_LineName);
        ui->tableSEGY->item(row,2)->setText(QString::number(segy->_TotalTraces));
        ui->tableSEGY->item(row,3)->setText(QString::number(segy->_SampleRate));
        ui->tableSEGY->item(row,4)->setText(QString::number(segy->_EndTime, 'f', 1));
        ui->tableSEGY->item(row,5)->setText(QString::number(segy->_FirstCDP));
        ui->tableSEGY->item(row,6)->setText(QString::number(segy->_LastCDP));
        ui->tableSEGY->item(row,7)->setText(QString::number(segy->_CDPInc,'f', 2));
        ui->tableSEGY->item(row,8)->setText(QString::number(segy->_FirstSP));
        ui->tableSEGY->item(row,9)->setText(QString::number(segy->_LastSP));
        ui->tableSEGY->item(row,10)->setText(QString::number(segy->_SPInc,'f', 2));
        ui->tableSEGY->item(row,11)->setText(QString::number(segy->_FirstX,'f', 1));
        ui->tableSEGY->item(row,12)->setText(QString::number(segy->_LastX,'f', 1));
        ui->tableSEGY->item(row,13)->setText(QString::number(segy->_FirstY,'f', 1));
        ui->tableSEGY->item(row,14)->setText(QString::number(segy->_LastY,'f', 1));
    }
    SetupTable();
    //QMessageBox::information(this, "Finish", "Finish   ", "OK");
    isAnalyzed = true;
}

void segyanalyzer::AddRow()
{
    int row = ui->tableSEGY->rowCount();
    ui->tableSEGY->insertRow(row);
    QTableWidgetItem *item0  = new QTableWidgetItem;item0->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);ui->tableSEGY->setItem(row,0,item0);
    QTableWidgetItem *item1  = new QTableWidgetItem;item1->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);ui->tableSEGY->setItem(row,1,item1);
    QTableWidgetItem *item2  = new QTableWidgetItem;item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,2,item2);
    QTableWidgetItem *item3  = new QTableWidgetItem;item3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,3,item3);
    QTableWidgetItem *item4  = new QTableWidgetItem;item4->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,4,item4);
    QTableWidgetItem *item5  = new QTableWidgetItem;item5->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,5,item5);
    QTableWidgetItem *item6  = new QTableWidgetItem;item6->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,6,item6);
    QTableWidgetItem *item7  = new QTableWidgetItem;item7->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,7,item7);
    QTableWidgetItem *item8  = new QTableWidgetItem;item8->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,8,item8);
    QTableWidgetItem *item9  = new QTableWidgetItem;item9->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,9,item9);
    QTableWidgetItem *item10 = new QTableWidgetItem;item10->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,10,item10);
    QTableWidgetItem *item11 = new QTableWidgetItem;item11->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,11,item11);
    QTableWidgetItem *item12 = new QTableWidgetItem;item12->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,12,item12);
    QTableWidgetItem *item13 = new QTableWidgetItem;item13->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,13,item13);
    QTableWidgetItem *item14 = new QTableWidgetItem;item14->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);ui->tableSEGY->setItem(row,14,item14);
}

void segyanalyzer::RunSetByte()
{
    setbyte *sb = new setbyte(this);
    sb->InitByte(segy->_ByteCDP, segy->_ByteSP, segy->_ByteX, segy->_ByteY, segy->_ByteLineName, segy->_LineNameLength, segy->_ByteInline, segy->_ByteXline);
    sb->exec();
    if(sb->result() == 1)
    {
        segy->SetByte(sb->byteCDP, sb->byteSP, sb->byteX, sb->byteY, sb->byteInline, sb->byteXline, sb->byteLineName, sb->byteLineNameLength);
        ui->lbByteLocation->setText("Line Name=<strong>" + QString::number(segy->_ByteLineName) + "</strong> , Line Name Length=<strong>" + QString::number(segy->_LineNameLength) + "</strong> , CDP=<strong>" + QString::number(segy->_ByteCDP) + "</strong> , SP=<strong>" + QString::number(segy->_ByteSP) + "</strong> , X=<strong>" + QString::number(segy->_ByteX) + "</strong> , Y=<strong>" + QString::number(segy->_ByteY) + "</strong> , Inline=<strong>" + QString::number(segy->_ByteInline) + "</strong> , Xline=<strong>" + QString::number(segy->_ByteXline) + "</strong>");
    }
}

void segyanalyzer::Print()
{
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    QString html;

    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("C:\\output.pdf");
    printer->setOrientation(QPrinter::Landscape);
    printer->setPaperSize(QPrinter::A4);
    printer->setPageMargins(5,5,5,5,QPrinter::Millimeter);

    html = "<table width='100%' cellspacing='0' cellpadding='0' border='0' style='font-family:Arial;'><tr><td align='left'>"+ui->lbDirPath->text()+"</td><td align='center'>"+ui->lbByteLocation->text()+"</td><td align='right'>"+ui->lbTotalFiles->text()+"</td></tr></table>\n"
           "<table width='100%' cellspacing='0' cellpadding='4' border='1' style='font-family:Arial; font-size:100px;'>\n"
           "<tr style='background-color:#CCC;' valign='middle'>"
           "<th>&nbsp;&nbsp;&nbsp;</th>"
           "<th>Filename</th>"
           "<th>Line Name</th>"
           "<th width='500px'>Total<br/>Traces</th>"
           "<th>Sample<br/>Rate</th>"
           "<th>End Time</th>"
           "<th>First CDP</th>"
           "<th>Last CDP</th>"
           "<th>CDP Inc.</th>"
           "<th>First SP</th>"
           "<th>Last SP</th>"
           "<th>SP Inc.</th>"
           "<th>First X</th>"
           "<th>Last X</th>"
           "<th>First Y</th>"
           "<th>Last Y</th>"
           "</tr>\n";

    for(int row=0; row<ui->tableSEGY->rowCount(); row++)
    {

        html += "<tr>"
                "<td>&nbsp;&nbsp;&nbsp;</td>"
                "<td>" + ui->tableSEGY->item(row,0)->text() + "</td>"
                "<td>" + ui->tableSEGY->item(row,1)->text() + "</td>"
                "<td align='right' width='500px'>" + ui->tableSEGY->item(row,2)->text() + "</td>"
                "<td align='center'>" + ui->tableSEGY->item(row,3)->text() + "</td>"
                "<td align='center'>" + ui->tableSEGY->item(row,4)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,5)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,6)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,7)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,8)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,9)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,10)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,11)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,12)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,13)->text() + "</td>"
                "<td align='right'>" + ui->tableSEGY->item(row,14)->text() + "</td>"
                "</tr>\n";
    }

    html += "</table>";

    //QPrintDialog printDialog(printer, this);
    //if (printDialog.exec()) {
        QTextDocument textDocument;
        textDocument.setHtml(html);
        textDocument.print(printer);
    //}

    QString path = QDir::toNativeSeparators("C:\\output.pdf");
    QDesktopServices::openUrl(QUrl("file:///" + path));
}
