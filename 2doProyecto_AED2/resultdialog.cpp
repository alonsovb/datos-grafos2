#include "resultdialog.h"
#include "ui_resultdialog.h"
#include "graph.h"

resultDialog::resultDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resultDialog)
{
    ui->setupUi(this);
}

resultDialog::~resultDialog()
{
    delete ui;
}

// Calcular la distancia de una sublista de arcos
int resultDialog::totalDistance(graph::sbarc *start) {
    int tdistance = 0;
    for (graph::sbarc *temp = start;
         temp != NULL; temp = temp->nxt) {
        tdistance += temp->dest->dist;
    }
    return tdistance;
}



resultDialog::resultDialog(graph::sbarc *start, int asig, int comp, int line,
                           int time, int mem, int pos, int brn, int pru, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::resultDialog)
{
    ui->setupUi(this);
    ui->listWidget->clear();
    if (start != NULL) {
        for (graph::sbarc *temp = start; temp != NULL; temp = temp->nxt) {
            ui->listWidget->addItem(QString::fromStdString(temp->dest->start->name) + " - " +
                                    QString::fromStdString(temp->dest->end->name) + " (" +
                                    QString::number(temp->dest->dist) + ")");
        }
        int distance = totalDistance(start);
        ui->listWidget->addItem("Total Distance: " + QString::number(distance));
    } else {
        ui->listWidget->addItem("No results");
    }
    ui->listResults->addItem("Asigs: " + QString::number(asig));
    ui->listResults->addItem("Comps: " + QString::number(comp));
    ui->listResults->addItem("Lines: " + QString::number(line));
    ui->listResults->addItem("Posibilities: " + QString::number(pos));
    ui->listResults->addItem("Branches: " + QString::number(brn));
    ui->listResults->addItem("Prunes: " + QString::number(pru));
    int _time = line * 13 + (time * 0);
    ui->listResults->addItem("Time:  " + QString::number(_time));
    ui->listResults->addItem("Memory:  " + QString::number(mem) + " bytes");
}

void resultDialog::on_buttonClose_pressed()
{
    this->close();
    this->~resultDialog();
}
