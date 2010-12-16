#ifndef SEGYANALYZER_H
#define SEGYANALYZER_H

#include <QWidget>
#include <segy.h>

namespace Ui {
    class segyanalyzer;
}

class segyanalyzer : public QWidget
{
    Q_OBJECT

public:
    explicit segyanalyzer(QWidget *parent = 0);
    ~segyanalyzer();

private:
    SEGY *segy;
    Ui::segyanalyzer *ui;
    QString selectedDir;
    bool isFileSelected;
    bool isByteSetted;
    bool isAnalyzed;
    void AddRow();
    void CenteredWindow();
    void SetupTable();

private slots:
    void SelectSEGYFiles();
    void ClearTable();
    void Analyze();
    void RunSetByte();
    void Print();
};

#endif // SEGYANALYZER_H
