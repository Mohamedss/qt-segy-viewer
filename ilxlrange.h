#ifndef ILXLRANGE_H
#define ILXLRANGE_H

#include <QDialog>

namespace Ui {
    class ilxlrange;
}

class ilxlrange : public QDialog
{
    Q_OBJECT

public:
    explicit ilxlrange(QWidget *parent = 0);
    ~ilxlrange();
    int byteInline;
    int byteXline;
    int byteX;
    int byteY;

private:
    Ui::ilxlrange *ui;

private slots:
    void onStart();
};

#endif // ILXLRANGE_H
