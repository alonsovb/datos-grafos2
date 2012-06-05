#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include <graph.h>
#include <mainwindow.h>

namespace Ui {
    class resultDialog;
}

class resultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit resultDialog(QWidget *parent = 0);

    resultDialog(graph::sbarc  *start, int asig, int comp, int line, int time, int mem, int pos, int brn,
                 int pru, QWidget *parent = 0);
    ~resultDialog();

private:
    Ui::resultDialog *ui;
    int totalDistance(graph::sbarc *start);

private slots:
    void on_buttonClose_pressed();
};

#endif // RESULTDIALOG_H
