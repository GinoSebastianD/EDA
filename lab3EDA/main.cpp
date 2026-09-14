#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Point {
    double x, y, z;

    Point() : x(0), y(0), z(0) {}
    Point(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

double distanceSquared(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

class Octree {
private:
    Point bottomLeft;
    double h;
    int capacity;
    vector<Point> points;
    Octree* children[8];

    bool isLeaf() const {
        for (int i = 0; i < 8; ++i) {
            if (children[i] != nullptr) return false;
        }
        return true;
    }

    bool contains(const Point& p) const {
        return p.x >= bottomLeft.x && p.x <= bottomLeft.x + h &&
               p.y >= bottomLeft.y && p.y <= bottomLeft.y + h &&
               p.z >= bottomLeft.z && p.z <= bottomLeft.z + h;
    }

    int getOctant(const Point& p) const {
        Point center(bottomLeft.x + h / 2.0, bottomLeft.y + h / 2.0, bottomLeft.z + h / 2.0);
        int octant = 0;
        if (p.x >= center.x) octant += 1;
        if (p.y >= center.y) octant += 2;
        if (p.z >= center.z) octant += 4;
        return octant;
    }

    Point childBottomLeft(int octant) const {
        double half = h / 2.0;
        return Point(
            bottomLeft.x + ((octant & 1) ? half : 0),
            bottomLeft.y + ((octant & 2) ? half : 0),
            bottomLeft.z + ((octant & 4) ? half : 0)
        );
    }

    void subdivide() {
        double half = h / 2.0;
        for (int i = 0; i < 8; ++i) {
            children[i] = new Octree(childBottomLeft(i), half, capacity);
        }

        vector<Point> oldPoints = points;
        points.clear();

        for (const Point& p : oldPoints) {
            int octant = getOctant(p);
            children[octant]->insert(p);
        }
    }

    bool cubeIntersectsSphere(const Point& center, double radius) const {
        double closestX = max(bottomLeft.x, min(center.x, bottomLeft.x + h));
        double closestY = max(bottomLeft.y, min(center.y, bottomLeft.y + h));
        double closestZ = max(bottomLeft.z, min(center.z, bottomLeft.z + h));
        Point closest(closestX, closestY, closestZ);
        return distanceSquared(center, closest) <= radius * radius;
    }

    void findClosestRec(const Point& query, double radius, Point& best, double& bestDist, bool& found) const {
        if (!cubeIntersectsSphere(query, radius)) return;

        if (isLeaf()) {
            for (const Point& p : points) {
                double d = distanceSquared(query, p);
                if (d <= radius * radius && (!found || d < bestDist)) {
                    best = p;
                    bestDist = d;
                    found = true;
                }
            }
            return;
        }

        for (int i = 0; i < 8; ++i) {
            if (children[i] != nullptr) {
                children[i]->findClosestRec(query, radius, best, bestDist, found);
            }
        }
    }

    void printRec(int level) const {
        for (int i = 0; i < level; ++i) cout << "  ";
        cout << "Nodo nivel " << level
             << " | bottomLeft=(" << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << ")"
             << " | lado=" << h
             << " | puntos=" << points.size()
             << (isLeaf() ? " | hoja" : " | interno")
             << "\n";

        for (int i = 0; i < 8; ++i) {
            if (children[i] != nullptr) children[i]->printRec(level + 1);
        }
    }

    void collectLeafCubes(vector<pair<Point, double>>& cubes) const {
        if (isLeaf()) {
            cubes.push_back({bottomLeft, h});
            return;
        }

        for (int i = 0; i < 8; ++i) {
            if (children[i] != nullptr) children[i]->collectLeafCubes(cubes);
        }
    }

public:
    Octree(Point bottomLeft_, double h_, int capacity_)
        : bottomLeft(bottomLeft_), h(h_), capacity(capacity_) {
        for (int i = 0; i < 8; ++i) children[i] = nullptr;
    }

    ~Octree() {
        for (int i = 0; i < 8; ++i) {
            delete children[i];
        }
    }

    bool exist(const Point& p) const {
        if (!contains(p)) return false;

        if (isLeaf()) {
            for (const Point& current : points) {
                if (current.x == p.x && current.y == p.y && current.z == p.z) return true;
            }
            return false;
        }

        int octant = getOctant(p);
        return children[octant]->exist(p);
    }

    void insert(const Point& p) {
        if (!contains(p)) return;
        if (exist(p)) return;

        if (isLeaf() && (int)points.size() < capacity) {
            points.push_back(p);
            return;
        }

        if (isLeaf()) subdivide();

        int octant = getOctant(p);
        children[octant]->insert(p);
    }

    bool find_closest(const Point& query, double radius, Point& answer) const {
        double bestDist = numeric_limits<double>::max();
        bool found = false;
        findClosestRec(query, radius, answer, bestDist, found);
        return found;
    }

    void printTree() const {
        cout << "Raiz bottomLeft=(" << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << ")\n";
        cout << "Lado de la raiz=" << h << "\n";
        printRec(0);
    }

    void exportOBJ(const string& fileName) const {
        vector<pair<Point, double>> cubes;
        collectLeafCubes(cubes);

        ofstream obj(fileName);
        if (!obj) {
            cout << "No se pudo crear el archivo " << fileName << "\n";
            return;
        }

        int vertexIndex = 1;
        for (const auto& cube : cubes) {
            Point b = cube.first;
            double s = cube.second;

            obj << "v " << b.x     << " " << b.y     << " " << b.z     << "\n";
            obj << "v " << b.x + s << " " << b.y     << " " << b.z     << "\n";
            obj << "v " << b.x + s << " " << b.y + s << " " << b.z     << "\n";
            obj << "v " << b.x     << " " << b.y + s << " " << b.z     << "\n";
            obj << "v " << b.x     << " " << b.y     << " " << b.z + s << "\n";
            obj << "v " << b.x + s << " " << b.y     << " " << b.z + s << "\n";
            obj << "v " << b.x + s << " " << b.y + s << " " << b.z + s << "\n";
            obj << "v " << b.x     << " " << b.y + s << " " << b.z + s << "\n";

            obj << "l " << vertexIndex     << " " << vertexIndex + 1 << "\n";
            obj << "l " << vertexIndex + 1 << " " << vertexIndex + 2 << "\n";
            obj << "l " << vertexIndex + 2 << " " << vertexIndex + 3 << "\n";
            obj << "l " << vertexIndex + 3 << " " << vertexIndex     << "\n";
            obj << "l " << vertexIndex + 4 << " " << vertexIndex + 5 << "\n";
            obj << "l " << vertexIndex + 5 << " " << vertexIndex + 6 << "\n";
            obj << "l " << vertexIndex + 6 << " " << vertexIndex + 7 << "\n";
            obj << "l " << vertexIndex + 7 << " " << vertexIndex + 4 << "\n";
            obj << "l " << vertexIndex     << " " << vertexIndex + 4 << "\n";
            obj << "l " << vertexIndex + 1 << " " << vertexIndex + 5 << "\n";
            obj << "l " << vertexIndex + 2 << " " << vertexIndex + 6 << "\n";
            obj << "l " << vertexIndex + 3 << " " << vertexIndex + 7 << "\n";

            vertexIndex += 8;
        }

        cout << "OBJ generado: " << fileName << "\n";
    }
};

vector<Point> readXYZ(const string& fileName) {
    vector<Point> points;
    ifstream file(fileName);

    if (!file) {
        cout << "No se pudo abrir " << fileName << "\n";
        return points;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        double x, y, z;
        if (ss >> x >> y >> z) {
            points.push_back(Point(x, y, z));
        }
    }

    return points;
}

Octree* buildOctree(const vector<Point>& points, int capacity) {
    if (points.empty()) return nullptr;

    double minX = points[0].x, minY = points[0].y, minZ = points[0].z;
    double maxX = points[0].x, maxY = points[0].y, maxZ = points[0].z;

    for (const Point& p : points) {
        minX = min(minX, p.x);
        minY = min(minY, p.y);
        minZ = min(minZ, p.z);
        maxX = max(maxX, p.x);
        maxY = max(maxY, p.y);
        maxZ = max(maxZ, p.z);
    }

    double side = max(maxX - minX, max(maxY - minY, maxZ - minZ));
    if (side == 0) side = 1;
    side += 0.001;

    Octree* tree = new Octree(Point(minX, minY, minZ), side, capacity);
    for (const Point& p : points) {
        tree->insert(p);
    }

    return tree;
}

int main() {
    cout << "Archivo .xyz: ";
    string fileName;
    cin >> fileName;

    cout << "Cantidad maxima de puntos por hoja: ";
    int capacity;
    cin >> capacity;

    vector<Point> points = readXYZ(fileName);
    if (points.empty()) {
        cout << "No hay puntos para insertar.\n";
        return 0;
    }

    Octree* tree = buildOctree(points, capacity);

    cout << "\nOctree construido con " << points.size() << " puntos.\n";
    tree->printTree();
    tree->exportOBJ("octree.obj");

    Point query;
    double radius;
    cout << "\nBuscar punto mas cercano\n";
    cout << "Ingrese x y z: ";
    cin >> query.x >> query.y >> query.z;
    cout << "Ingrese radio: ";
    cin >> radius;

    Point answer;
    if (tree->find_closest(query, radius, answer)) {
        cout << "Punto mas cercano: (" << answer.x << ", " << answer.y << ", " << answer.z << ")\n";
    } else {
        cout << "No se encontro ningun punto dentro del radio.\n";
    }

    delete tree;
    return 0;
}
