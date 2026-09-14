#include "Octree.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

Octree::Octree() : Octree(Point(0, 0, 0), 1, 4) {}

Octree::Octree(Point esquina, double lado, int limite)
    : bottomLeft(esquina), h(lado), nPoints(0), capacidad(limite) {
    if (lado <= 0 || limite < 1) {
        throw invalid_argument("El lado y la capacidad deben ser positivos.");
    }
    for (int i = 0; i < 8; i++) {
        children[i] = nullptr;
    }
}

Octree::~Octree() {
    for (int i = 0; i < 8; i++) {
        delete children[i];
    }
}

bool Octree::contiene(const Point& p) {
    return p.x >= bottomLeft.x && p.x <= bottomLeft.x + h &&
           p.y >= bottomLeft.y && p.y <= bottomLeft.y + h &&
           p.z >= bottomLeft.z && p.z <= bottomLeft.z + h;
}

int Octree::octante(const Point& p) {
    int posicion = 0;
    if (p.x >= bottomLeft.x + h / 2) posicion += 1;
    if (p.y >= bottomLeft.y + h / 2) posicion += 2;
    if (p.z >= bottomLeft.z + h / 2) posicion += 4;
    return posicion;
}

void Octree::dividir() {
    double mitad = h / 2;
    for (int i = 0; i < 8; i++) {
        Point esquina = bottomLeft;
        if (i % 2 == 1) esquina.x += mitad;
        if ((i / 2) % 2 == 1) esquina.y += mitad;
        if (i >= 4) esquina.z += mitad;
        children[i] = new Octree(esquina, mitad, capacidad);
    }

    for (const Point& p : points) {
        children[octante(p)]->insert(p);
    }
    points.clear();
}

bool Octree::exist(const Point& p) {
    if (!contiene(p)) return false;

    if (children[0] != nullptr) {
        return children[octante(p)]->exist(p);
    }

    for (const Point& actual : points) {
        if (actual.x == p.x && actual.y == p.y && actual.z == p.z) {
            return true;
        }
    }
    return false;
}

void Octree::insert(const Point& p) {
    if (!contiene(p)) {
        throw out_of_range("El punto esta fuera del cubo raiz.");
    }
    if (exist(p)) return;

    if (children[0] == nullptr) {
        if (static_cast<int>(points.size()) < capacidad || h <= 1e-9) {
            points.push_back(p);
            nPoints++;
            return;
        }
        dividir();
    }

    children[octante(p)]->insert(p);
    nPoints++;
}

double Octree::distanciaCubo(const Point& p) {
    double dx = 0, dy = 0, dz = 0;

    if (p.x < bottomLeft.x) dx = bottomLeft.x - p.x;
    else if (p.x > bottomLeft.x + h) dx = p.x - (bottomLeft.x + h);
    if (p.y < bottomLeft.y) dy = bottomLeft.y - p.y;
    else if (p.y > bottomLeft.y + h) dy = p.y - (bottomLeft.y + h);
    if (p.z < bottomLeft.z) dz = bottomLeft.z - p.z;
    else if (p.z > bottomLeft.z + h) dz = p.z - (bottomLeft.z + h);

    return dx * dx + dy * dy + dz * dz;
}

void Octree::buscar(const Point& p, double& mejorDistancia, Point& mejor, bool& encontrado) {
    if (nPoints == 0 || distanciaCubo(p) > mejorDistancia) return;

    if (children[0] == nullptr) {
        for (const Point& actual : points) {
            double dx = actual.x - p.x;
            double dy = actual.y - p.y;
            double dz = actual.z - p.z;
            double distancia = dx * dx + dy * dy + dz * dz;

            if (distancia <= mejorDistancia) {
                mejorDistancia = distancia;
                mejor = actual;
                encontrado = true;
            }
        }
    } else {
        for (int i = 0; i < 8; i++) {
            children[i]->buscar(p, mejorDistancia, mejor, encontrado);
        }
    }
}

Point Octree::find_closest(const Point& p, int radius) {
    if (radius < 0) {
        throw invalid_argument("El radio no puede ser negativo.");
    }

    double mejorDistancia = static_cast<double>(radius) * radius;
    Point mejor;
    bool encontrado = false;
    buscar(p, mejorDistancia, mejor, encontrado);

    if (!encontrado) {
        throw runtime_error("No hay puntos dentro del radio indicado.");
    }
    return mejor;
}

void Octree::print(ostream& salida, int depth) {
    if (nPoints == 0) return;

    for (int i = 0; i < depth; i++) salida << "  ";
    salida << setprecision(12) << "nivel=" << depth;
    salida << " bottomLeft=(" << bottomLeft.x << ", " << bottomLeft.y;
    salida << ", " << bottomLeft.z << ") lado=" << h;
    salida << " puntos=" << nPoints;
    salida << (children[0] == nullptr ? " hoja\n" : " interno\n");

    if (children[0] != nullptr) {
        for (int i = 0; i < 8; i++) {
            children[i]->print(salida, depth + 1);
        }
    }
}

void Octree::escribirCubo(ostream& archivo, int& vertices) {
    double x = bottomLeft.x, y = bottomLeft.y, z = bottomLeft.z;

    archivo << "v " << x << " " << y << " " << z << '\n';
    archivo << "v " << x + h << " " << y << " " << z << '\n';
    archivo << "v " << x + h << " " << y + h << " " << z << '\n';
    archivo << "v " << x << " " << y + h << " " << z << '\n';
    archivo << "v " << x << " " << y << " " << z + h << '\n';
    archivo << "v " << x + h << " " << y << " " << z + h << '\n';
    archivo << "v " << x + h << " " << y + h << " " << z + h << '\n';
    archivo << "v " << x << " " << y + h << " " << z + h << '\n';

    int caras[6][4] = {
        {1, 4, 3, 2}, {5, 6, 7, 8}, {1, 2, 6, 5},
        {2, 3, 7, 6}, {3, 4, 8, 7}, {4, 1, 5, 8}
    };

    for (int i = 0; i < 6; i++) {
        archivo << "f";
        for (int j = 0; j < 4; j++) archivo << " " << vertices + caras[i][j];
        archivo << '\n';
    }
    vertices += 8;
}

void Octree::escribirHojas(ostream& archivo, int& vertices) {
    if (nPoints == 0) return;

    if (children[0] == nullptr) {
        escribirCubo(archivo, vertices);
    } else {
        for (int i = 0; i < 8; i++) {
            children[i]->escribirHojas(archivo, vertices);
        }
    }
}

void Octree::exportarOBJ(const string& nombre, bool soloRaiz) {
    ofstream archivo(nombre);
    if (!archivo) throw runtime_error("No se pudo crear " + nombre);
    archivo << setprecision(17);
    int vertices = 0;

    if (soloRaiz) escribirCubo(archivo, vertices);
    else escribirHojas(archivo, vertices);

    archivo.close();
    if (!archivo) throw runtime_error("No se pudo completar " + nombre);
}
