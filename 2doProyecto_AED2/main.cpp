#include <QtGui/QApplication>
#include "mainwindow.h"

#include "graph.h"
#include "string"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    graph *g = new graph();

    MainWindow *w = new MainWindow(g);
    w->show();

    return a.exec();
}
