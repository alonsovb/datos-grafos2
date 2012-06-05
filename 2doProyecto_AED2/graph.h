#ifndef GRAPH_H
#define GRAPH_H

#include "string"

using namespace std;

struct graph
{
public:
    graph();

    struct vert;
    struct sbvert;
    struct arc;
    struct sbarc;
    struct obj;

    // Estructuras para el grafo del problema del viajero

    /* Vertice: Representa un lugar, con su nombre, ID y marca */
    struct vert {
        int id;         // Numero identificador de objeto
        string name;    // Nombre del lugar (vertice)
        bool mark;      // Marca temporal en procedimientos
        sbarc *alist;   // Lista de arcos que lo apuntan
        vert *nxt;      // Siguiente elemento de la lista

        /* Constructor del vertice
            Recibe un valor numerico de ID y un texto de nombre */
        vert(int _id, string _name) {
            id = _id;
            name = _name;
            mark = false;
            alist = NULL;
            nxt = NULL;
        }

        /* Insertar un arco en la lista de subarcos */
        void insArc(arc *narc) {
            sbarc *nsbarc = new sbarc(narc);
            if (alist == NULL)
                alist = nsbarc;
            else {
                nsbarc->nxt = alist;
                alist = nsbarc;
            }
        }
    };

    /* Sub-Vertice: Usado para crear sublistas de vertices */
    struct sbvert {
        vert *dest;
        sbvert *nxt;
        sbvert(vert *_dest) {
            dest = _dest;
            nxt = NULL;
        }
    };

    /* Arco: Representa un camino, con su ID, la distancia y destinos */
    struct arc {
        int id;         // Numero identificador de objeto
        int dist;       // Distancia de la ruta (arco)
        vert *start;    // Vertice de salida
        vert *end;      // Vertice de llegada
        arc *nxt;       // Siguiente elemento de la lista
        bool mark;       // Marca temporal
        arc(int _id, int _dist, vert *_start, vert *_end) {
            id = _id;
            dist = _dist;
            start = _start;
            end = _end;
            nxt = NULL;
            mark = false;
        }
        vert *getOther(vert *initial) {
            if (initial == start)
                return end;
            else if (initial == end)
                return start;
            else
                return NULL;
        }
    };

    /* Sub-Arco: Usado para crear sublistas de arcos */
    struct sbarc {
        arc *dest;
        sbarc *nxt;
        sbarc(arc *_dest) {
            dest = _dest;
            nxt = NULL;
        }
    };

    /* Estructura para el problema de la mochila */

    /* Objeto: Posee un ID, nombre, peso y valor de beneficio */
    struct obj {
        int id;         // Numero identificador de objeto
        string name;    // Nombre del objeto
        int weight;     // Peso del objeto
        int benefit;    // Valor de beneficio del objeto
        obj *nxt;       // Siguiente elemento de la lista
        bool mark;      // Marca temporal usada por los algoritmos
        obj(int _id, string _name, int _weight, int _benefit) {
            id = _id;
            name = _name;
            weight = _weight;
            benefit = _benefit;
            nxt = NULL;
            mark = false;
        }
    };

    // Sub-objeto, para crear listas de objetos
    struct sbobj {
        obj *dest;      // Objeto destino
        sbobj *nxt;     // Siguiente elemento
    };

    bool readFromFile(string fileName);

    // Inicios de listas
    vert *startv;       // Primer vertice
    arc *starta;        // Primer arco
    obj *starto;        // Primer objeto

    int arcc;
    int vertc;
    int objc;

    vert *findVert(int id);
    void clearMarks();
    void clearAMarks();
    void clearOMarks();

private:
    void clearStarts();

    bool processVert(string line);
    bool processArc(string line);
    bool processObj(string line);

    // Insercion en la lista enlazada
    void insertVert(vert *nvert);
    void insertArc(arc *narc);
    void insertObj(obj *nobj);
};

#endif // GRAPH_H
