#include <iostream>
using namespace std;

int matriz[4][4] = {
    {1, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

struct Node {
    int valor;
    Node* hijos[4] = { nullptr, nullptr, nullptr, nullptr };

    ~Node() {
        for (int i = 0; i < 4; i++) {
            delete hijos[i];
        }
    }
};

bool isUniform(int x, int y, int size) {
    int valor = matriz[x][y];
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            if (matriz[i][j] != valor) {
                return false;
            }
        }
    }
    return true;
}

Node* buildRecursive(int x, int y, int size) {
    Node* node = new Node;
    if (isUniform(x, y, size)) {
        node->valor = matriz[x][y];
    }
    else {
        node->valor = -1;
        int mitad = size / 2;
        node->hijos[0] = buildRecursive(x, y, mitad);
        node->hijos[1] = buildRecursive(x, y + mitad, mitad);
        node->hijos[2] = buildRecursive(x + mitad, y, mitad);
        node->hijos[3] = buildRecursive(x + mitad, y + mitad, mitad);
    }
    return node;
}

void printRecursive(Node* node, int depth, int x, int y, int size) {
    for (int i = 0; i < depth; i++) {
        cout << "    ";
    }
    cout << "(fila=" << x << ", columna=" << y << ") tam=" << size;
    if (node->valor != -1) {
        cout << " valor=" << node->valor << '\n';
    }
    else {
        cout << " dividida\n";
        int mitad = size / 2;
        printRecursive(node->hijos[0], depth + 1, x, y, mitad);
        printRecursive(node->hijos[1], depth + 1, x, y + mitad, mitad);
        printRecursive(node->hijos[2], depth + 1, x + mitad, y, mitad);
        printRecursive(node->hijos[3], depth + 1, x + mitad, y + mitad, mitad);
    }
}

int main() {
    Node* raiz = buildRecursive(0, 0, 4);
    printRecursive(raiz, 0, 0, 0, 4);
    delete raiz;
    return 0;
}
