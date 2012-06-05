
#include "graph.h"
#include "string"
#include "fstream"
#include "string.h"
#include "cstdlib"

#include "iostream"

// Insertar vertice en la lista enlazada
// Recibe el vertice nuevo que se insertara
void graph::insertVert(vert *nvert) {
    if (startv != NULL) {
        nvert->nxt = startv;
        startv = nvert;
    } else {
        startv = nvert;
    }
    vertc++;
}

// Insertar arco en la lista enlazada
// Recibe el arco nuevo que se insertara
void graph::insertArc(arc *narc) {
    if (starta != NULL) {
        narc->nxt = starta;
        starta = narc;
    } else {
        starta = narc;
    }
}

// Insertar objeto en la lista enlazada
// Recibe el objeto nuevo que se insertara
void graph::insertObj(obj *nobj) {
    if (starto != NULL) {
        if (starto->benefit < nobj->benefit) {
            nobj->nxt = starto;
            starto = nobj;
        } else {
            obj *temp = starto;
            while (temp->nxt != NULL &&
                   temp->nxt->benefit > nobj->benefit)
                temp = temp->nxt;
            nobj->nxt = temp->nxt;
            temp->nxt = nobj;
        }
    } else {
        starto = nobj;
    }
}

graph::graph() {
    startv = NULL;
    starta = NULL;
    starto = NULL;
    arcc = 0;
    vertc = 0;
    objc = 0;
}

void graph::clearStarts() {
    startv = NULL;
    starta = NULL;
    starto = NULL;
    arcc = 0;
    vertc = 0;
    objc = 0;
}

void graph::clearMarks() {
    for (vert *temp = startv; temp != NULL; temp = temp->nxt)
        temp->mark = false;
}

void graph::clearAMarks() {
    for (arc *temp = starta; temp != NULL; temp = temp->nxt)
        temp->mark = false;
}

void graph::clearOMarks() {
    for (obj *temp = starto; temp != NULL; temp = temp->nxt) {
        temp->mark = false;
    }
}

/*
 Buscar y devolver un vertice.
 Recibe el entero con el ID buscado.
 Se devuelve el vertice encontrado, o NULL si no se encuentra.
 */
graph::vert *graph::findVert(int id) {
    for (vert *temp = startv; temp != NULL; temp = temp->nxt) {
        if (temp->id == id)
            return temp;
    }
    return NULL;
}

/*
 Leer el contenido de un archivo e interpretarlo como un grafo.
 Es decir, un conjunto de vertices y arcos. Ademas el archivo puede contener
 informacion sobre objetos, para ser utilizada en el problema de la mochila.
 De los vertices se lee un identificador (ID) y un nombre.
 De los arcos se lee la distancia que representan, y el vertice de salida y
 llegada.
 De los objetos se lee el nombre, el peso y el beneficio.
 Se recibe un string con el nombre del archivo que se leerá.
 */
bool graph::readFromFile(string fileName) {

    // abrir el archivo en modo lectura
    fstream file;
    file.open(fileName.c_str(), ios_base::in);

    /* Cargar un grafo de un nuevo archivo provoca que los objetos anteriores
       sean eliminados y sustituidos por los datos leidos del archivo. */
    clearStarts();

    // Comprobar que el archivo se abrió correctamente
    if (file.is_open()) {

        /* Se comprueba el largo del archivo moviendo el puntero de lectura
           hasta el final del archivo y leyendo la posicion actual */
        int fileLen = 0;
        file.seekg(0, ios_base::end);
        fileLen = file.tellg();

        /* Se lee el contenido del archivo en un arreglo de caracteres */
        char fileContent[fileLen];
        file.seekg(0);
        file.read(fileContent, fileLen);

        try {
            int last = 0, type = 0;
            for (int i = 0; i < fileLen; i++) {
                if (fileContent[i] == ':') {
                    string line = "";
                    line.append(fileContent, last, i - last);
                    last = i + 1;
                    switch (type) {
                    case 0:
                        if (!processVert(line)) {
                            clearStarts();
                            return false;
                        }
                        break;
                    case 1:
                        if (!processArc(line)) {
                            clearStarts();
                            return false;
                        }
                        break;
                    case 2:
                        if (!processObj(line)) {
                            clearStarts();
                            return false;
                        }
                    }
                    type++;
                }
            }
        } catch (exception e) {
            clearStarts();
            return false;
        }
    }
    return false;
}

bool graph::processVert(string line) {
    try {
        int last = 0;
        for (unsigned int i = 0; i < line.length(); i++) {
            if (line.at(i) == ';') {
                string sub = line.substr(last, i - last + 1);

                int id; string name;
                int slast = 0, type = 0;
                for (unsigned int j = 0; j < sub.length(); j++) {
                    if (sub.at(j) == ',' || sub.at(j) == ';' || j == sub.length() - 1) {
                        switch (type) {
                        case 0:
                            id = atoi(sub.substr(slast, j - slast).c_str());
                            break;
                        case 1:
                            name = sub.substr(slast, j - slast);
                            break;
                        default:
                            break;
                        }
                        slast = j + 1;
                        type++;
                    }
                }

                vert *nvert = new vert(id, name);
                insertVert(nvert);
                last = i + 1;
            }
        }
        return true;
    } catch (exception e) {
        return false;
    }
}

bool graph::processArc(string line) {
    try {
        int last = 0;
        for (unsigned int i = 0; i < line.length(); i++) {
            if (line.at(i) == ';') {
                string sub = line.substr(last, i - last + 1);

                int dist; vert *start, *end;
                int slast = 0, type = 0;
                for (unsigned int j = 0; j < sub.length(); j++) {
                    if (sub.at(j) == ',' || sub.at(j) == ';' || j == sub.length() - 1) {
                        switch (type) {
                        case 0:
                            dist = atoi(sub.substr(slast, j - slast).c_str());
                            break;
                        case 1:
                            start = findVert(atoi(sub.substr(slast, j - slast).c_str()));
                            break;
                        case 2:
                            end = findVert(atoi(sub.substr(slast, j - slast).c_str()));
                            break;
                        default:
                            break;
                        }
                        slast = j + 1;
                        type++;
                    }
                }
                if (start != NULL && end != NULL) {
                    arc *narc = new arc(arcc++, dist, start, end);
                    insertArc(narc);
                    start->insArc(narc);
                    end->insArc(narc);
                }
                last = i + 1;
            }
        }
        return true;
    } catch (exception e) {
        return false;
    }
}

bool graph::processObj(string line) {
    try {
        int last = 0;
        for (unsigned int i = 0; i < line.length(); i++) {
            if (line.at(i) == ';') {
                string sub = line.substr(last, i - last + 1);

                int benefit, weight; string name;
                int slast = 0, type = 0;
                for (unsigned int j = 0; j < sub.length(); j++) {
                    if (sub.at(j) == ',' || sub.at(j) == ';' || j == sub.length() - 1) {
                        switch(type) {
                        case 0:
                            name = sub.substr(slast, j - slast);
                            break;
                        case 1:
                            weight = atoi(sub.substr(slast, j - slast).c_str());
                            break;
                        case 2:
                            benefit = atoi(sub.substr(slast, j - slast).c_str());
                            break;
                        }
                        slast = j + 1;
                        type++;
                    }
                }
            obj *nobj = new obj(objc++, name, weight, benefit);
            insertObj(nobj);
            last = i + 1;
            }
        }
        return true;
    } catch (exception e) {
        return false;
    }
}


