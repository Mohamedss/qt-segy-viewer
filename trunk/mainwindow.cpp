#include <QtGui>
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <segyanalyzer.h>
#include <ilxlrange.h>

FILE* _F = NULL;
char  _EbcdicHeader [3200];
char  _BinaryHeader [400];
char  _TraceHeader  [240];
char  _INPTRC       [10000];
float _Data         [10000];
double _SampleInterval;
int _TotalSamples;
int _DataFormat;
int _SampleRate;
int _TRL;
int _TotalTraces;
int _CurrentTrace;

QString viewMode;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    segy = new SEGY();

    CenteredWindow();

    ui->tabWidget->setVisible(false);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSEGY_Analyzer, SIGNAL(triggered()), this, SLOT(runSEGYAnalyzer()));
    connect(ui->btnFirstTrace, SIGNAL(clicked()), this, SLOT(goFirstTrace()));
    connect(ui->btnLastTrace, SIGNAL(clicked()), this, SLOT(goLastTrace()));
    connect(ui->btnNextTrace, SIGNAL(clicked()), this, SLOT(goNextTrace()));
    connect(ui->btnPrevTrace, SIGNAL(clicked()), this, SLOT(goPrevTrace()));
    connect(ui->spinboxTrace, SIGNAL(valueChanged(int)), this, SLOT(goToTrace(int)));
    connect(ui->actionFind_Min_Max_Inline_Xline, SIGNAL(triggered()), this, SLOT(runIlXlRange()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CenteredWindow()
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

void MainWindow::goToTrace(int)
{
    segy->ShowTraceHeader(ui->spinboxTrace->value(), ui->ptxtTrace);
}

void MainWindow::goFirstTrace()
{
    ui->spinboxTrace->setValue(1);
}

void MainWindow::goLastTrace()
{
    ui->spinboxTrace->setValue(segy->_TotalTraces);
}

void MainWindow::goPrevTrace()
{
    ui->spinboxTrace->setValue(ui->spinboxTrace->value() - ui->spinBox->value());
}

void MainWindow::goNextTrace()
{
    ui->spinboxTrace->setValue(ui->spinboxTrace->value() + ui->spinBox->value());
}

void MainWindow::runSEGYAnalyzer()
{
    segyanalyzer *sa = new segyanalyzer();
    sa->setWindowModality(Qt::WindowModal);
    sa->show();
}

void MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(
            this,
            tr("Open SEGY"),
            (recentPath.isNull())? QDir::currentPath():recentPath,
            tr("SEGY (*.sgy *.segy)"),
            0
    );

    if(!filename.isNull())
    {
        QDir dir = QFileInfo(filename).absolutePath();
        recentPath = dir.absolutePath();
        openSEGY(filename);
    }
}

void MainWindow::openSEGY(QString filename)
{
    if(segy->OpenFile(filename.toAscii().data()))
    {
        setWindowTitle("2D SEGY Viewer - [" + filename + "]");

        segy->ShowEbcdicHeader(ui->ptxtEbcdic);
        segy->ShowBinaryHeader(ui->ptxtBinary);
        segy->ShowTraceHeader(1, ui->ptxtTrace);

        ui->btnFirstTrace->setEnabled(true);
        ui->btnLastTrace->setEnabled(true);
        ui->spinboxTrace->setEnabled(true);
        ui->spinboxTrace->setMaximum(segy->_TotalTraces);
        ui->spinboxTrace->setValue(1);
        ui->tabWidget->setVisible(true);

        ui->ptxtSummary->setPlainText(
                "Path :..............." + filename.replace("/","\\") + "\n" +
                "Total Traces :......." + QString::number(segy->_TotalTraces) + "\n" +
                "Sample Rate :........" + QString::number(segy->_SampleRate) + " s\n" +
                "Sample Interval :...." + QString::number(segy->_SampleInterval) + " ms\n"
                "Total Samples :......" + QString::number(segy->_TotalSamples)
        );
    }
}

void MainWindow::runIlXlRange()
{
    if(segy->FileIsOpen())
    {
        ilxlrange *ilxl = new ilxlrange();
        ilxl->exec();
        if(ilxl->result() == 1)
        {
            int il,xl;
            il = ilxl->byteInline;
            xl = ilxl->byteXline;

            //Read at _ByteInline and from 1 to _TotalTraces
            int minInline, maxInline, minXline, maxXline, tmp;
            minInline = maxInline = minXline = maxXline = tmp = 0;

            QElapsedTimer timer;
            timer.start();
            for(int trace=1; trace<=segy->_TotalTraces; trace++)
            {
                tmp = segy->Read4Byte(trace, il);
                if(minInline == 0 && maxInline == 0) { minInline = tmp; maxInline = tmp; }
                minInline = (tmp < minInline)?  tmp:minInline;
                maxInline = (tmp > maxInline)?  tmp:maxInline;
                tmp = segy->Read4Byte(trace, xl);
                if(minXline == 0 && maxXline == 0) { minXline = tmp; maxXline = tmp; }
                minXline = (tmp < minXline)?  tmp:minXline;
                maxXline = (tmp > maxXline)?  tmp:maxXline;
            }
            long ms = timer.elapsed();
            QTime time(0,0,0,ms);
            QMessageBox::information(this,
                 "Result",
                 "Min Inline = " + QString::number(minInline) + "\n" +
                 "Max Inline = " + QString::number(maxInline) + "\n" +
                 "Min Xline = " + QString::number(minXline) + "\n" +
                 "Max Xline = " + QString::number(maxXline) + "\n\n" +
                 "Time used = " + time.toString("hh mm ss")
            );
        }
    }
    else
    {
        QMessageBox::critical(this,"Error","SEGY file is not opened.");
    }
}
