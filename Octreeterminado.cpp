#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>

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
    int capacity;
    int level = 0;

    bool isLeaf() const {
        return children[0] == nullptr;
    }

    bool inBounds(const Point& p) const {
        return p.x >= bottomLeft.x && p.x <= bottomLeft.x + h &&
               p.y >= bottomLeft.y && p.y <= bottomLeft.y + h &&
               p.z >= bottomLeft.z && p.z <= bottomLeft.z + h;
    }

    int getOctant(const Point& p) const {
        double midX = bottomLeft.x + h / 2.0;
        double midY = bottomLeft.y + h / 2.0;
        double midZ = bottomLeft.z + h / 2.0;

        int octant = 0;
        if (p.x >= midX) octant |= 1;
        if (p.y >= midY) octant |= 2;
        if (p.z >= midZ) octant |= 4;
        return octant;
    }

    void subdivide() {
        double half = h / 2.0;

        for (int i = 0; i < 8; i++) {
            int dx = (i & 1) ? 1 : 0;
            int dy = (i & 2) ? 1 : 0;
            int dz = (i & 4) ? 1 : 0;

            Point childBL(
                bottomLeft.x + dx * half,
                bottomLeft.y + dy * half,
                bottomLeft.z + dz * half
            );
            children[i] = new Octree(childBL, half, capacity);
            children[i]->level = level + 1;
        }

        for (const Point& p : points) {
            int octant = getOctant(p);
            children[octant]->insert(p);
        }
        points.clear();
    }

    double distance(const Point& a, const Point& b) const {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        double dz = a.z - b.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    void collectLeaves(std::vector<const Octree*>& leaves) const {
        if (isLeaf()) {
            if (nPoints > 0) leaves.push_back(this);
            return;
        }
        for (int i = 0; i < 8; i++) {
            children[i]->collectLeaves(leaves);
        }
    }

public:
    Octree(Point bl, double side, int cap = 1)
        : bottomLeft(bl), h(side > 0 ? side : 1), nPoints(0), capacity(cap > 0 ? cap : 1) {
        for (int i = 0; i < 8; i++) children[i] = nullptr;
    }

    // Evita copiar punteros y liberar la misma memoria dos veces.
    Octree(const Octree&) = delete;
    Octree& operator=(const Octree&) = delete;

    ~Octree() {
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr) {
                delete children[i];
            }
        }
    }

    bool exist(const Point& p) {
        if (!inBounds(p)) return false;

        if (isLeaf()) {
            for (const Point& q : points) {
                if (q.x == p.x && q.y == p.y && q.z == p.z) return true;
            }
            return false;
        }

        int octant = getOctant(p);
        return children[octant]->exist(p);
    }

    void insert(const Point& p) {
        if (!inBounds(p)) return;

        if (isLeaf()) {
            // Si ya no conviene dividir, guardamos los puntos en esta hoja.
            // Esto permite conservar repetidos sin una recursion infinita.
            double half = h / 2.0;
            bool puedeDividir = level < 48 && h > 0.000001 &&
                bottomLeft.x + half > bottomLeft.x &&
                bottomLeft.y + half > bottomLeft.y &&
                bottomLeft.z + half > bottomLeft.z;
            if (nPoints < capacity || !puedeDividir) {
                points.push_back(p);
                nPoints++;
            } else {
                subdivide();
                int octant = getOctant(p);
                children[octant]->insert(p);
                nPoints++;
            }
        } else {
            int octant = getOctant(p);
            children[octant]->insert(p);
            nPoints++;
        }
    }

    Point find_closest(const Point& p, int radius) {
        Point best(0, 0, 0);
        double bestDist = std::numeric_limits<double>::max();

        std::vector<const Octree*> leaves;
        collectLeaves(leaves);

        for (const Octree* leaf : leaves) {
            for (const Point& q : leaf->points) {
                double d = distance(p, q);
                if (d <= radius && d < bestDist) {
                    bestDist = d;
                    best = q;
                }
            }
        }

        return best;
    }

    void printTree(int depth = 0) const {
        for (int i = 0; i < depth; i++) std::cout << "  ";
        std::cout << "Nodo bottomLeft(" << bottomLeft.x << ", " << bottomLeft.y
                  << ", " << bottomLeft.z << ") h=" << h
                  << " nPoints=" << nPoints << (isLeaf() ? " [hoja]" : "") << "\n";

        if (!isLeaf()) {
            for (int i = 0; i < 8; i++) {
                children[i]->printTree(depth + 1);
            }
        }
    }

    void printRootInfo() const {
        std::cout << "Lado del nodo raiz: " << h << "\n";
        std::cout << "bottomLeft del nodo raiz: (" << bottomLeft.x << ", "
                  << bottomLeft.y << ", " << bottomLeft.z << ")\n";
    }

    void exportOBJ(const std::string& filename) const {
        std::vector<const Octree*> leaves;
        collectLeaves(leaves);

        std::ofstream file(filename);
        if (!file) {
            std::cerr << "No se pudo crear: " << filename << "\n";
            return;
        }
        int vertexOffset = 0;

        for (const Octree* leaf : leaves) {
            double x0 = leaf->bottomLeft.x;
            double y0 = leaf->bottomLeft.y;
            double z0 = leaf->bottomLeft.z;
            double s = leaf->h;

            double verts[8][3] = {
                {x0,     y0,     z0},
                {x0 + s, y0,     z0},
                {x0 + s, y0 + s, z0},
                {x0,     y0 + s, z0},
                {x0,     y0,     z0 + s},
                {x0 + s, y0,     z0 + s},
                {x0 + s, y0 + s, z0 + s},
                {x0,     y0 + s, z0 + s}
            };

            for (int i = 0; i < 8; i++) {
                file << "v " << verts[i][0] << " " << verts[i][1] << " " << verts[i][2] << "\n";
            }

            int f[6][4] = {
                {1, 2, 3, 4},
                {5, 6, 7, 8},
                {1, 2, 6, 5},
                {2, 3, 7, 6},
                {3, 4, 8, 7},
                {4, 1, 5, 8}
            };

            for (int i = 0; i < 6; i++) {
                file << "f " << vertexOffset + f[i][0] << " " << vertexOffset + f[i][1]
                     << " " << vertexOffset + f[i][2] << " " << vertexOffset + f[i][3] << "\n";
            }

            vertexOffset += 8;
        }

        file.close();
    }
};

std::vector<Point> loadPoints(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "No se pudo abrir: " << filename << "\n";
        return points;
    }
    double x, y, z;

    while (file >> x >> y >> z) {
        if (std::isfinite(x) && std::isfinite(y) && std::isfinite(z)) {
            points.push_back(Point(x, y, z));
        }
    }

    return points;
}

int main() {
    std::vector<Point> points = loadPoints("puntos1.xyz");

    if (points.empty()) {
        std::cerr << "No hay puntos validos para construir el octree.\n";
        return 1;
    }

    double minX = points[0].x, maxX = points[0].x;
    double minY = points[0].y, maxY = points[0].y;
    double minZ = points[0].z, maxZ = points[0].z;

    for (const Point& p : points) {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
        if (p.z < minZ) minZ = p.z;
        if (p.z > maxZ) maxZ = p.z;
    }

    double side = maxX - minX;
    if (maxY - minY > side) side = maxY - minY;
    if (maxZ - minZ > side) side = maxZ - minZ;
    side += 1;

    Point bottomLeft(minX, minY, minZ);
    int capacity = 4;

    Octree tree(bottomLeft, side, capacity);

    for (const Point& p : points) {
        tree.insert(p);
    }

    tree.printRootInfo();
    tree.printTree();

    Point testPoint(minX, minY, minZ);
    std::cout << "exist(bottomLeft): " << (tree.exist(testPoint) ? "true" : "false") << "\n";

    Point closest = tree.find_closest(testPoint, 10);
    std::cout << "Punto mas cercano: (" << closest.x << ", " << closest.y << ", " << closest.z << ")\n";

    tree.exportOBJ("octree.obj");

    return 0;
}
