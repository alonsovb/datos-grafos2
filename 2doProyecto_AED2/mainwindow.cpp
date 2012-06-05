#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "resultdialog.h"

#include <math.h>
#include "cstdlib"
#include "cstdio"
#include <QFileDialog>
#include <QTime>
#include <QMessageBox.h>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "graph.h"

using namespace std;

MainWindow::MainWindow(graph *_g, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    g = _g;
    circleSize = 100;
    loadFile();
    asig = 0;
    comp = 0;
    line = 0;
    time = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *re) {
    ShowGraph();
}

void hover(QGraphicsSceneHoverEvent *hevent) {

}

// Dibujar el grafo
void MainWindow::ShowGraph() {

    QGraphicsScene *scene = new QGraphicsScene();
    QPen pen(QColor(200, 200, 200));
    pen.setWidth(3);
    QBrush brush(QColor(255, 128, 125));

    circleSize = ui->graphicsView->height() / 10;

    double pi = atan(1)*4;
    int halfw = (ui->graphicsView->width()  / 2) - circleSize - 40,
        halfh = (ui->graphicsView->height() / 2) - circleSize - 10,
               vcount = g->vertc, i = 0;
    double partang = pi * 2/ vcount;

    // Linea de arcos
    for (graph::arc *temp = g->starta; temp != NULL; temp = temp->nxt) {
        if (temp->mark) {
            pen.setColor(QColor(255, 0, 0));
        } else {
            pen.setColor(QColor(200, 200, 200));
        }
        int sx = (cos(temp->start->id * partang) *  halfw) + circleSize / 2,
            sy = (sin(temp->start->id * partang) * -halfh) + circleSize / 2,
            ex = (cos(temp->end->id * partang) *  halfw) + circleSize / 2,
            ey = (sin(temp->end->id * partang) * -halfh) + circleSize / 2;
        scene->addLine(sx, sy, ex, ey, pen);

        // Texto
        if (ui->checkADist->isChecked()) {
            QGraphicsSimpleTextItem *ntext =
                    scene->addSimpleText(QString::number(temp->dist));
            ntext->setX((sx + ex) / 2);
            ntext->setY((sy + ey) / 2);
        }
    }

    pen.setWidth(2);
    for (graph::vert *temp = g->startv;
         temp != NULL; temp = temp->nxt) {

        i = temp->id;

        // Brush (color) para dibujar el circulo
        brush.setColor(QColor(32, (128 / vcount * i) + 64, (191 / vcount * i) + 64));

        // Valores de posicion
        double actualang = i * partang;
        int x = cos(actualang) *  halfw, y = sin(actualang) * -halfh;

        // Circulo
        QGraphicsEllipseItem *item =
                scene->addEllipse(x, y, circleSize, circleSize, pen, brush);

        // Texto
        if (ui->checkVName->isChecked()) {
            QGraphicsSimpleTextItem *ntext =
                    scene->addSimpleText(QString::fromStdString(temp->name));
            ntext->setX(x + circleSize);
            ntext->setY(y);
        }

        item->setAcceptHoverEvents(true);
    }
    ui->graphicsView->setScene(scene);
}

// Leer de un archivo
void MainWindow::loadFile() {
    QString filename;
    // Mostrar el dialgo y obtener el nombre de archivo
    filename = QFileDialog::getOpenFileName(
            this,
            "Seleccione el archivo",
            QDir::currentPath(),
            "Archivo de grafo (*.txt *.grph);;Todos los archivos (*.*)");
    if (!filename.isNull()) {
        g->readFromFile(filename.toStdString());
        ui->vertList->clear();
        ui->comboStart->clear();
        ui->arcList->clear();
        ui->objList->clear();

        if (g->startv == NULL)
            ui->vertList->addItem("No vertexes");
        else {
            graph::vert *temp = g->startv;
            while (temp != NULL) {
                ui->vertList->addItem(QString::fromStdString(temp->name));
                graph::sbarc *sbtemp = temp->alist;
                while (sbtemp != NULL) {
                    ui->vertList->addItem("- " + QString::fromStdString(sbtemp->dest->getOther(temp)->name));
                    sbtemp = sbtemp->nxt;
                }

                ui->comboStart->addItem(QString::fromStdString(temp->name));
                temp = temp->nxt;
            }
        }
        if (g->starta == NULL)
            ui->arcList->addItem("No arcs");
        else {
            for (graph::arc *temp = g->starta; temp != NULL; temp = temp->nxt) {
                QString tarc = QString::fromStdString(temp->start->name) +
                               " - " + QString::fromStdString(temp->end->name);
                ui->arcList->addItem(tarc);
            }
        }
        if (g->starto == NULL)
            ui->objList->addItem("No items");
        else {
            int tbenefit = 0;
            for (graph::obj *temp = g->starto; temp != NULL; temp = temp->nxt) {
                tbenefit += temp->benefit;
                ui->objList->addItem(QString::fromStdString(temp->name) + " - Weight: " +
                                     QString::number(temp->weight) + " - Benefit: " +
                                     QString::number(temp->benefit));
            }
            ui->objList->addItem("Total benefit: " + QString::number(tbenefit));
        }
        ShowGraph();
    }
}

void MainWindow::on_pushButton_clicked()
{
    loadFile();
}

void MainWindow::on_checkADist_toggled(bool checked)
{
    ShowGraph();
}

void MainWindow::on_checkVName_toggled(bool checked)
{
    ShowGraph();
}

void MainWindow::on_buttonSolve_pressed()
{
    int id = g->vertc - ui->comboStart->currentIndex() - 1;
    graph::vert *start = g->findVert(id);
    g->clearMarks();
    g->clearAMarks();
    resetCounters();

    graph::sbarc *result;

    // Resolver mediante el metodo voraz
    if (ui->radioGreedy->isChecked()) {
        QTime t = QTime(0, 0, 0, 0);
        t.start();
        result = MainWindow::travelerGreedy(start);
        line = comp + asig;
        time = t.elapsed();
        // Metodo de back tracking
    } else if (ui->radioBackT->isChecked()) {
        QTime t = QTime(0, 0, 0, 0);
        int prev[] = { id };
        result = travelerBackT(1, prev, NULL);
        line = comp + asig;
        time = t.elapsed();
        // Metodo de branching and prouning
    } else if (ui->radioBranch->isChecked()) {
        QTime t = QTime(0, 0, 0, 0);
        int prev[] = { id };
        result = travelerBranch(1, prev, NULL);
        line = comp + asig;
        time = t.elapsed();
    } else {
        return;
    }
    // Marcar los arcos utilizados en el algoritmo seleccionado
    for (graph::sbarc *temp = result; temp != NULL; temp = temp->nxt) {
        temp->dest->mark = true;
    }
    ShowGraph();

    resultDialog *rd = new resultDialog(result, asig, comp, line, time, mem,
                                        posi, bran, prun, this);
    rd->setModal(true);
    rd->show();
}

/* Metodo del viajero por algoritmo voraz
   Recibe el vertice de inicio, y devuelve una lista con los arcos
   de las rutas mas cortas
   */
graph::sbarc *MainWindow::travelerGreedy(graph::vert *start) {

    if (start != NULL) {
        // Variable de resultado final
        graph::sbarc *result = NULL; mem += sizeof(result);

        // temporal para recorrer grafo
        graph::vert *temp = start; mem += sizeof(temp);

        // total de marcas, para comparar al final si se marcaron todas las ciudades
        int tmarks = 1; mem += sizeof(tmarks);
        start->mark = true;
        asig += 4;
        // revisar cada vertice hasta que no quede opcion de moverse
        do {
            graph::arc *nxtRoad = NULL; asig += 2; comp++;
            mem += sizeof(nxtRoad);
            // comprobar en la lista de rutas, la mas corta
            for (graph::sbarc *nxtTemp = temp->alist;
                 nxtTemp != NULL; nxtTemp = nxtTemp->nxt) {
                asig++; comp += 2;
                // Si es mas corta o no hay ninguna otra actualmente
                if (!nxtTemp->dest->getOther(temp)->mark &&
                    (nxtRoad == NULL || nxtTemp->dest->dist < nxtRoad->dist)) {
                    asig++;
                    // Se asigna como siguiente
                    nxtRoad = nxtTemp->dest;
                }
            }
            comp++;
            // Si se logro encontrar una ruta
            if (nxtRoad != NULL) {
                temp = nxtRoad->getOther(temp);
                temp->mark = true;
                tmarks++;
                comp++;
                // Insertar en la lista de resultado
                if (result != NULL) {
                    graph::sbarc *stemp = result;
                    comp++; asig++;
                    while (stemp->nxt != NULL) {
                        comp++; asig++;
                        stemp = stemp->nxt;
                    }
                    asig++;
                    stemp->nxt = new graph::sbarc(nxtRoad);
                    mem += sizeof(graph::sbarc);
                } else {
                    asig++;
                    result = new graph::sbarc(nxtRoad);
                    mem += sizeof(graph::sbarc);
                }
                asig += 3;
            } else {
                comp++;
                // Si se marcaron todos los vertices, se devuelve el resultado
                if (tmarks == g->vertc)
                    return result;
                // De otra manera, se devuelve nulo
                else
                    return NULL;
            }
            comp++;
        } while (temp != start);
        return result;
    } else
        return NULL;
}

// Calcular la distancia de una sublista de arcos
int MainWindow::totalDistance(graph::sbarc *start) {
    int tdistance = 0;
    mem += sizeof(tdistance);
    for (graph::sbarc *temp = start;
         temp != NULL; temp = temp->nxt) {
        tdistance += temp->dest->dist;
    }
    return tdistance;
}

/* Metodo de Back Tracking para el problema del viajero
   Recibe pcount = total de elementos en el arreglo de entrada
   pindexes = arreglo con los indices de los vertices ya recorridos
   */
graph::sbarc *MainWindow::travelerBackT(int pcount, int pindexes[], graph::arc *last) {

    // El elemento actual se encuentra al final de la lista
    graph::vert *actual = g->findVert(pindexes[pcount - 1]);
    mem += sizeof(actual);

    // Arco con el vertice mas cercano
    graph::sbarc *closerarc = NULL;
    mem += sizeof(closerarc);

    asig += 3;
    // Revisar en cada ruta del vertice actual
    comp++;
    for (graph::sbarc *branch = actual->alist; branch != NULL; branch = branch->nxt) {
        comp++;
        bool listed = false;
        graph::vert *dest = branch->dest->getOther(actual);
        mem += sizeof(dest);
        asig += 2;
        for (int i = 0; i < pcount; i++) {
            // Buscar las rutas que lleguen a lugares que ya estan listados
            comp += 2;
            if (g->findVert(pindexes[i]) == dest) {
                listed = true;
                asig++;
                break;
            }
        }
        // Si un camino no está listado
        comp++;
        if (!listed) {
            // Si se esta en el penultimo nivel, donde solo falta un elemento
            comp++;
            if (pcount == g->vertc - 1) {
                graph::sbarc *tres = new graph::sbarc(last);
                mem += sizeof(graph::sbarc);
                tres->nxt = new graph::sbarc(branch->dest);
                mem += sizeof(graph::sbarc);
                asig += 2; posi++;
                return tres;
            } else {
                /* Copiar el arreglo original y agregarle un elemento
                   indice de la ruta procesada */
                int *nindexes;
                nindexes = new int[pcount + 1];
                mem += sizeof(nindexes);
                asig += 3; comp++;
                for (int i = 0; i < pcount; i++) {
                    nindexes[i] = pindexes[i];
                    asig++; comp++;
                }
                nindexes[pcount] = dest->id;

                // Obtener una solucion con la llamada recursiva
                graph::sbarc *narc = travelerBackT(pcount + 1, nindexes, branch->dest);
                mem += sizeof(narc);
                asig += 2;
                comp++;
                // Si se obtiene una solucion nula, se devuelve nulo
                if (narc != NULL) {

                    comp++;
                    // Si no se ha planteado solucion o se encuentra una mas corta
                    if (closerarc == NULL ||
                        totalDistance(narc) < totalDistance(closerarc))
                        // se selecciona la nueva solucion
                        asig++;
                        closerarc = narc;
                }
            }
        }
    }
    // Si no se encontro solucion se retorna nulo
    comp++;
    if (closerarc == NULL) {
        return NULL;
    }
    // Si hay rama antecesora se agrega y se devuelve
    comp++;
    if (last != NULL) {
        graph::sbarc *tres = new graph::sbarc(last);
        mem += sizeof(graph::sbarc);
        tres->nxt = closerarc;
        asig += 2;
        return tres;
    } else {
        // De otra forma solo se devuelve la ruta encontrada
        return closerarc;
    }
}

graph::sbarc *MainWindow::travelerBranch(int pcount, int pindexes[], graph::arc *last) {
    // El elemento actual se encuentra al final de la lista
    graph::vert *actual = g->findVert(pindexes[pcount - 1]);
    mem += sizeof(actual);

    // Arco con el vertice mas cercano
    graph::sbarc *closerarc = NULL;
    mem += sizeof(closerarc);

    asig += 3;
    // Revisar en cada ruta del vertice actual
    comp++;
    for (graph::sbarc *branch = actual->alist; branch != NULL; branch = branch->nxt) {
        comp++;
        bool listed = false;
        graph::vert *dest = branch->dest->getOther(actual);
        bran++;
        mem += sizeof(dest);
        asig += 1;
        for (int i = 0; i < pcount; i++) {
            // Buscar las rutas que lleguen a lugares que ya estan listados
            comp++;
            if (g->findVert(pindexes[i]) == dest) {
                listed = true;
                asig++;
                break;
            }
        }
        // Si un camino no está listado
        comp++;
        if (!listed) {
            // Si se esta en el penultimo nivel, donde solo falta un elemento
            comp++;
            if (pcount == g->vertc - 1) {
                graph::sbarc *tres = new graph::sbarc(last);
                mem += sizeof(graph::sbarc);
                tres->nxt = new graph::sbarc(branch->dest);
                asig += 2;
                return tres;
            } else {
                /* Copiar el arreglo original y agregarle un elemento
                   indice de la ruta procesada */
                int *nindexes;
                nindexes = new int[pcount + 1];
                asig += 3; comp++;
                for (int i = 0; i < pcount; i++) {
                    nindexes[i] = pindexes[i];
                    asig++; comp++;
                }
                nindexes[pcount] = dest->id;

                // Obtener una solucion con la llamada recursiva
                graph::sbarc *narc = travelerBranch(pcount + 1, nindexes, branch->dest);
                mem += sizeof(narc);
                comp++;
                // Si se obtiene una solucion nula, se devuelve nulo
                if (narc != NULL) {

                    comp += 2;
                    // Si no se ha planteado solucion o se encuentra una mas corta
                    if (closerarc == NULL ||
                        totalDistance(narc) < totalDistance(closerarc))
                        // se selecciona la nueva solucion
                        asig++;
                        closerarc = narc;
                }
            }
        } else {
            prun++;
        }
    }
    // Si no se encontro solucion se retorna nulo
    comp++;
    if (closerarc == NULL) {
        return NULL;
    }
    // Si hay rama antecesora se agrega y se devuelve
    comp++;
    if (last != NULL) {
        graph::sbarc *tres = new graph::sbarc(last);
        mem += sizeof(graph::sbarc);
        tres->nxt = closerarc;
        asig += 2;
        return tres;
    } else {
        // De otra forma solo se devuelve la ruta encontrada
        return closerarc;
    }
}

void MainWindow::on_buttonBPGreedy_pressed()
{
    // Limpiar listas y contadores
    int maxWeight = ui->spinMaxWeight->value();
    g->clearOMarks();
    ui->listBest->clear();
    ui->listMeasures->clear();
    resetCounters();

    int tweight = 0, tbenefit = 0, count = 0;

    if (ui->radioBPGreedy->isChecked()) {
        backpackGreedy(maxWeight);
    } else if (ui->radioDynamic->isChecked()) {
        MainWindow::pd(maxWeight);
    } else {
        return;
    }
    line = comp + asig;
    // Imprimir el resultado obtenido a las listas
    graph::obj *temp = g->starto;
    while (temp != NULL) {
        if (temp->mark) {
            tweight += temp->weight;
            tbenefit += temp->benefit;
            count++;
            ui->listBest->addItem(QString::fromStdString(temp->name) + " - Weight: " +
                                  QString::number(temp->weight) + " - Benefit: " +
                                  QString::number(temp->benefit));
        }
        temp = temp->nxt;
    }
    ui->listBest->addItem("Total items: " + QString::number(count));
    ui->listBest->addItem("Total benefit: " + QString::number(tbenefit));
    ui->listBest->addItem("Total weight: " + QString::number(tweight));

    ui->listMeasures->addItem("Asigs: " + QString::number(asig));
    ui->listMeasures->addItem("Comps: " + QString::number(comp));
    ui->listMeasures->addItem("Lines: " + QString::number(line));
    int _time = line * 13;
    ui->listMeasures->addItem("Time:  " + QString::number(_time));
    ui->listMeasures->addItem("Memory:  " + QString::number(mem) + " bytes");

    ui->progressBar->setMaximum(maxWeight);
    ui->progressBar->setValue(tweight);

}

//Voraz en mochila, recibe el peso
void MainWindow::backpackGreedy(int maxw) {
    // Peso maximo que se puede alcanzar
    int actualw = maxw;
    mem += sizeof(actualw);
    asig += 2;
    // Recorrer la lista de objetos, ya ordenada
    comp++;
    for (graph::obj *temp = g->starto; temp != NULL; temp = temp->nxt) {
        comp += 2;
        // Si el peso es suficiente para meterlo en la mochila, se agrega
        if (temp->weight <= actualw) {
            temp->mark = true;
            actualw -= temp->weight;
            asig += 2;
        }
    }
}
