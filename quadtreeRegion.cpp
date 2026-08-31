#include <iostream>
#include <vector>
using namespace std;

struct Node {
    vector<Node*> quad = vector<Node*>(4, nullptr);
    int valor;
    int x, y;
    int size;
    Node(int v, int _x, int _y, int _s) : valor(v), x(_x), y(_y), size(_s) {}
    bool isLeaf() { return quad[0] == nullptr; }
};

class RegionQuadTree {
public:
    int limite;
    Node* root = nullptr;
    vector<vector<int>> grid;

    RegionQuadTree(int lim) : limite(lim), grid(lim, vector<int>(lim, 0)) {}

    void build();
    void print();

private:
    bool isUniform(int x, int y, int size);
    Node* buildRecursive(int x, int y, int size);
    void printRecursive(Node* node);
};

bool RegionQuadTree::isUniform(int x, int y, int size) {
    int value = grid[x][y];
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            if (grid[i][j] != value) {
                return false;
            }
        }
    }
    return true;
}

void RegionQuadTree::printRecursive(Node* node) {
    if (!node) return;

    if (node->isLeaf()) {
        cout << "Hoja: (" << node->x << "," << node->y
            << ") = " << node->valor << "\n";
    }
    else {
        cout << "Nodo: (" << node->x << "," << node->y
            << "), size = " << node->size << "\n";
    }

    for (int i = 0; i < 4; i++) {
        printRecursive(node->quad[i]);
    }
}

Node* RegionQuadTree::buildRecursive(int x, int y, int size) {
    if (size == 1 || isUniform(x, y, size)) {
        Node* newNode = new Node(grid[x][y], x, y, size);
        return newNode;
    }

    Node* newNode = new Node(-1, x, y, size);
    int half = size / 2;
    newNode->quad[0] = buildRecursive(x, y, half);
    newNode->quad[1] = buildRecursive(x + half, y, half);
    newNode->quad[2] = buildRecursive(x, y + half, half);
    newNode->quad[3] = buildRecursive(x + half, y + half, half);

    return newNode;
}



void RegionQuadTree::build() {
    root = nullptr;
    root = buildRecursive(0, 0, limite);
}



void RegionQuadTree::print() {
    printRecursive(root);
}

int main() {
    RegionQuadTree qt(4);

    qt.grid = {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 1, 1},
        {0, 0, 1, 1}
    };

    qt.build();
    qt.print();

    return 0;
}
















//#include "iostream"
//#include "vector"
//
//using namespace std;
//
//
//struct Node
//{
//	vector<Node*> quad = vector<Node*>(4, nullptr);
//	int v;
//	int x;
//	int y;
//	int size;
//	Node(int val,  int x_, int y_ , int tam ): v(val) , x(x_) , y(y_) , size(tam) {}
//	bool eshoja() {
//		return quad[0] == nullptr;
//	}
//};
//
//class QuadtreeRegion
//{
//public:
//
//	vector<vector<int>> matriz;
//	int limite;
//	Node* raiz = nullptr;
//
//	QuadtreeRegion(int l) : limite(l) , matriz(l,vector<int>(l,0)){}
//
//	void build();
//	bool esigual(int x, int y , int size);
//	Node* recursividad_build(int x, int y, int val);
//	void set(int x, int y, int size);
//
//	void print();
//	void print_recursivo(Node* root);
//};
//
//
//void QuadtreeRegion::build()
//{
//	raiz = nullptr;
//	raiz = recursividad_build(0,0,limite);
//}
//bool QuadtreeRegion::esigual(int x, int y, int size)
//{
//	int val = matriz[x][y];
//	for (int i = x ; i < x + size ; i++)
//	{
//		for (int j = y; j < y + size ; j++)
//		{
//			if (val != matriz[i][j])
//			{
//				return false;
//			}
//		}
//	}
//	return true;
//}
//Node* QuadtreeRegion::recursividad_build(int x, int y, int size)
//{
//	
//	if (size == 1 || esigual(x,y,size))
//	{
//		Node* newNode = new Node(matriz[x][y], x, y, size);
//		return newNode;
//	}
//	Node* newNode = new Node(-1, x, y, size);
//	int medio = size / 2;
//	newNode->quad[0] = recursividad_build(x, y, medio);
//	newNode->quad[1] = recursividad_build(x + medio, y, medio);
//	newNode->quad[2] = recursividad_build(x, y + medio, medio);
//	newNode->quad[3] = recursividad_build(x + medio, y + medio, medio);
//	
//	return newNode;
//
//}
//
//void QuadtreeRegion::set(int x, int y, int val)
//{
//	matriz[x][y] = val;
//	build();
//}
//
//void QuadtreeRegion::print()
//{
//	print_recursivo(raiz);
//}
//
//void QuadtreeRegion::print_recursivo(Node* root)
//{
//	if (!root)
//	{
//		return;
//	}
//	if (root->eshoja())
//	{
//		cout << "Hoja " << "(" << root->x << "," << root->y << ") :" << root->v << "\n";
//	}
//	else
//	{
//		cout <<"Nodo: " << "(" << root->x << "," << root->y << ") :" <<"size: " << root->size << "\n";
//	}
//	for (int i = 0 ; i < 4; i++)
//	{
//		print_recursivo(root->quad[i]);
//	}
//}
//
//
//
//
//int main() {
//
//	QuadtreeRegion qt(4);
//	qt.matriz = {
//	{ 1, 0, 0, 0 },
//	{1, 1, 1, 0},
//	{1, 0, 1, 1},
//	{0, 0, 1, 1}
//	};
//
//	qt.build();
//	qt.print();
//}


































