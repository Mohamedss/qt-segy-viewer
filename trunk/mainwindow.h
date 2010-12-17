#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <segy.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow(); 
    void openSEGY(QString);

private:
    SEGY *segy;
    Ui::MainWindow *ui;
    QString recentPath;
    void CenteredWindow();

private slots:
    void openFile();
    void goLastTrace();
    void goNextTrace();
    void goPrevTrace();
    void goFirstTrace();
    void goToTrace(int);
    void runSEGYAnalyzer();
    void runIlXlRange();
};

#endif // MAINWINDOW_H
