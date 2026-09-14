#ifndef OCTREE_H
#define OCTREE_H

#include <iostream>
#include <string>
#include <vector>

struct Point {
    double x, y, z;

    Point(double a = 0, double b = 0, double c = 0) : x(a), y(b), z(c) {}
};

class Octree {
private:
    Octree* children[8];
    std::vector<Point> points;
    Point bottomLeft;
    double h;
    int nPoints;
    int capacidad;

    bool contiene(const Point& p);
    int octante(const Point& p);
    void dividir();
    double distanciaCubo(const Point& p);
    void buscar(const Point& p, double& mejorDistancia, Point& mejor, bool& encontrado);
    void escribirCubo(std::ostream& archivo, int& vertices);
    void escribirHojas(std::ostream& archivo, int& vertices);

public:
    Octree();
    Octree(Point esquina, double lado, int limite);
    ~Octree();

    bool exist(const Point& p);
    void insert(const Point& p);
    Point find_closest(const Point& p, int radius);
    void print(std::ostream& salida = std::cout, int depth = 0);
    void exportarOBJ(const std::string& nombre, bool soloRaiz = false);
};

#endif
