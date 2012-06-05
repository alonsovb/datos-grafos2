#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graph.h"
#include "string"
#include "math.h"
#include <QResizeEvent>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(graph *_g, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    graph *g;
    graph::sbvert *backtRes;
    void ShowGraph();
    int circleSize;
    void resizeEvent(QResizeEvent *re);
    void loadFile();

    void resetCounters() {
        asig = 0; comp = 0; line = 0; time = 0; mem = 0;
        posi = 0; bran = 0; prun = 0;
    }

    // Mediciones
    int asig, comp, line, time, mem;
    int posi, bran, prun;
    // ----------

    graph::sbarc *travelerGreedy(graph::vert *start);
    graph::sbarc *travelerBackT(int pcount, int pindexes[], graph::arc *last);
    graph::sbarc *travelerBranch(int pcount, int pindexes[], graph::arc *last);

    void backpackGreedy(int maxw);


    int totalDistance(graph::sbarc *start);

    // Convertir un numero a una cadena de caracteres binaria

    string decimal_binario(int n, int e)
    {
        asig += 5;
        string s = "";
        char *cs = new char[e];
        mem += sizeof(cs);
        itoa(n, cs, 2);
        s = cs;
        string ts;
        ts = "";
        comp++;
        // Llenar de ceros el resto del string
        for (int i = s.length(); i < e; i++)
        {
            comp++; asig++;
            ts.append("0");
        }
        ts.append(s);
        return ts;
    }

    // Marca la conbinación en la lista, para luego ser revisada
    // Recibe la combinacion en string, de pasar un numero a string
    void marcar_combinacion(string s)
    {
        comp += 3;
        int i = 0;
        mem += sizeof(i);
        graph::obj *temp = g->starto;
        mem += sizeof(temp);
        comp++;
        // Revisar cada combinacion
        for (int j = s.length(); j > 0 ; j--)
        {
            comp += 2; asig++;
            string num = s.substr(i,1);
            mem += sizeof(num);
            // El 1 indica una marca positiva
            if (num.compare("1") == 0)
                temp->mark = true;
            else
                temp->mark = false;
            temp = temp->nxt;
            i++;
            asig += 3;
        }
    }

    // Funcion principal para programacion dinamica, recibe el peso
    // Agregar la libreria #include "math.h"
    // s:solucion temporal
    // solución: solucion optima
    // b: mejor optimo
    void pd(int peso)
    {
        g->clearOMarks();
        int b = 0;
        string solucion;
        string s;
        int c = g->objc;
        asig += 3;
        for (int i = 1; i < pow(2, c); i++)
        {
            comp++;
            g->clearOMarks();

            s = decimal_binario(i, c);
            mem += sizeof(s);
            marcar_combinacion(s); //marcar
            int btemp = 0;
            int ptemp = 0;
            mem += 2 * sizeof(int);
            asig += 3; comp++;
            // Recorrer la lista de objetos
            for (graph::obj *temp = g->starto; temp != NULL; temp = temp->nxt)
            {
                comp++;
                // Comprobar si el objeto no esta marcado
                if (temp->mark != false)
                {
                    btemp = btemp + temp->benefit;
                    ptemp = ptemp + temp->weight;
                    asig += 2;
                }
            }
            comp++;
            // Si el peso es menor y el beneficio mayor
            if (ptemp <= peso && btemp > b)
            {
                // El objeto se agrega como solucion
                b = btemp;
                solucion = s;
                asig += 2;
            }
            btemp = 0;
            ptemp = 0;
            asig += 2;
        }
        marcar_combinacion(solucion);
    }

private slots:
    void on_buttonBPGreedy_pressed();
    void on_buttonSolve_pressed();
    void on_checkVName_toggled(bool checked);
    void on_checkADist_toggled(bool checked);
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
