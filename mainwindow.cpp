#include <QtGui>
#include <QThread>
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <segyanalyzer.h>
#include <ilxlrange.h>

#ifndef QT_NO_CONCURRENT

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
    connect(ui->actionFind_Min_Max_Inline_Xline, SIGNAL(triggered()), this, SLOT(runILXLRange()));
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

void MainWindow::runILXLRange()
{
    if(segy->FileIsOpen())
    {
        if(segy->_MinInline != 0 && segy->_MaxInline != 0 && segy->_MinXline != 0 && segy->_MaxXline != 0)
        {
            int result = QMessageBox::warning(this,"Already","Already, again?",QMessageBox::Yes,QMessageBox::No);
        }
        else
        {
            ilxlrange *ilxl = new ilxlrange();
            ilxl->exec();
            if(ilxl->result() == 1)
            {
                segy->_ByteInline = ilxl->byteInline;
                segy->_ByteXline  = ilxl->byteXline;
                segy->_ByteX      = ilxl->byteX;
                segy->_ByteY      = ilxl->byteY;

                // Create a progress dialog.
                QProgressDialog *dialog = new QProgressDialog();
                dialog->setLabelText(QString("Progressing using %1 thread(s)...").arg(QThread::idealThreadCount()));

                // Create a QFutureWatcher and conncect signals and slots.
                QFutureWatcher<void> *futureWatcher = new QFutureWatcher<void>;
                QObject::connect(futureWatcher, SIGNAL(finished()), dialog, SLOT(accept()));
                QObject::connect(dialog, SIGNAL(canceled()), futureWatcher, SLOT(cancel()));
                QObject::connect(futureWatcher, SIGNAL(progressRangeChanged(int,int)), dialog, SLOT(setRange(int,int)));
                QObject::connect(futureWatcher, SIGNAL(progressValueChanged(int)), dialog, SLOT(setValue(int)));

                // Start the computation.
                futureWatcher->setFuture(QtConcurrent::run(segy,&SEGY::computeILXLRange));

                // Display the dialog and start the event loop.
                dialog->exec();
                futureWatcher->waitForFinished();

                QMessageBox::information(this,
                     "Result",
                     "First Inline = " + QString::number(segy->_MinInline) + "\n" +
                     "Last  Inline = " + QString::number(segy->_MaxInline) + "\n" +
                     "First Xline  = " + QString::number(segy->_MinXline) + "\n" +
                     "Last  Xline  = " + QString::number(segy->_MaxXline) + "\n" +
                     "Corner#1 IL=" + QString::number(segy->_MinInline) + " XL=" + QString::number(segy->_MinXline) + " X1=" + QString::number(segy->_Corner1_X) + " Y1=" + QString::number(segy->_Corner1_Y) + "\n" +
                     "Corner#2 IL=" + QString::number(segy->_MinInline) + " XL=" + QString::number(segy->_MaxXline) + " X2=" + QString::number(segy->_Corner2_X) + " Y2=" + QString::number(segy->_Corner2_Y) + "\n" +
                     "Corner#3 IL=" + QString::number(segy->_MaxInline) + " XL=" + QString::number(segy->_MinXline) + " X3=" + QString::number(segy->_Corner3_X) + " Y3=" + QString::number(segy->_Corner3_Y)
                );

                ui->ptxtSummary->appendPlainText("First Inline : " + QString::number(segy->_MinInline));
                ui->ptxtSummary->appendPlainText("Last  Inline : " + QString::number(segy->_MaxInline));
                ui->ptxtSummary->appendPlainText("First Xline  : " + QString::number(segy->_MinXline));
                ui->ptxtSummary->appendPlainText("Last  Xline  : " + QString::number(segy->_MaxXline));
                ui->ptxtSummary->appendPlainText("Corner#1 IL=" + QString::number(segy->_MinInline) + " XL=" + QString::number(segy->_MinXline) + " X1=" + QString::number(segy->_Corner1_X) + " Y1=" + QString::number(segy->_Corner1_Y));
                ui->ptxtSummary->appendPlainText("Corner#2 IL=" + QString::number(segy->_MinInline) + " XL=" + QString::number(segy->_MaxXline) + " X2=" + QString::number(segy->_Corner2_X) + " Y2=" + QString::number(segy->_Corner2_Y));
                ui->ptxtSummary->appendPlainText("Corner#3 IL=" + QString::number(segy->_MaxInline) + " XL=" + QString::number(segy->_MinXline) + " X3=" + QString::number(segy->_Corner3_X) + " Y3=" + QString::number(segy->_Corner3_Y));
            }
        }
    }
    else
    {
        QMessageBox::critical(this,"Error","SEGY file is not opened.");
    }
}

#endif
