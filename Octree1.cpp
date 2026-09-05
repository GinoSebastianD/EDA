
#include "iostream"
#include "vector"
#include "algorithm"
#include "cmath"
#include "fstream"

using namespace std;


struct Point
{
	float x, y, z;
	Point() : x(0) , y(0) , z(0) {}
	Point(float a , float b , float c):x(a) , y(b) , z(c){}
};


vector<Point> loadXYZ(const string& filename) {
	vector<Point> pts; //vector de tipo Point
	ifstream file(filename);
	float x, y, z;
	while (cin>>x >> y >> z ) // el archivo tiene 3 columnas de numeros 
	{
		pts.push_back(Point(x,y,z));
	}
	cout << "Puntos cargados: " << pts.size() << endl;
	return pts;
}



class Octree
{
public:
	Octree *children[8]; //array de 8 punteros de tipo octree
	vector<Point> points;
	Point bottomleft;
	double h;
	int capacity;
	int nPoints = 0;
	
	Octree(double _h, int _c, Point bl) : h(_h), capacity(_c), bottomleft(bl) {
		for (int i = 0 ; i < 8; i++)
		{
			children[i] = nullptr;
		}
	};
	
	void insert(const Point& p);
	bool search(const Point& p , Octree*& node);

};



void Octree::insert(const Point& p)
{
	Octree* node = this; 
	
	if (search(p,node))
	{
		Octree* node = this;
    	if (search(p, node)) { node->nPoints++; node->points.push_back(p); }
    	else { split(node->points, node); insert(p); }
	}
}

bool Octree::search(const Point& p, Octree*& node) // un puntero al nodo actual del Octree
{
	while (node->children[0] != nullptr) //desciente mientras el nodo actual tenga hijos.
	{
		Point centro(node->bottomleft.x + node->h / 2, node->bottomleft.y + node->h / 2, node->bottomleft.z + node->h / 2);
		if (p.x < centro.x && p.y < centro.y && p.z < centro.z)
		{

		}
	}

}




int main() {
	const string ARCHIVO = "aguila.xyz";
	const int CAPACITY = 15; //limite maximo de puntos que un nodo hoja puede almacenar antes de dividirse en 8subnodos.

	vector<Point> pts = loadXYZ(ARCHIVO);
	if (pts.empty())
	{
		return 1;
	}

	float minX = pts[0].x;
	float minY = pts[0].y;
	float minZ = pts[0].z;

	float maxX = pts[0].x;
	float maxY = pts[0].y;
	float maxZ = pts[0].z;

	for (auto& p: pts){ //comparamos los puntos actuales con el resto para hallar los limites del bounding box(caja dlimitadora)
		minX = min(minX, p.x);
		minY = min(minY, p.y);
		minZ = min(minZ, p.z);

		maxX = max(maxX, p.x);
		maxY = max(maxY, p.y);
		maxZ = max(maxZ, p.z);
	}

	double h = max({ (double)(maxX - minX) ,(double)(maxY - minY) , (double)(maxZ - minZ) }) * 1.01; //calcular la dimension mas grande

	Point b1(minX, minY, minZ);
	Octree tree(h, CAPACITY, b1);
	for (auto &p : pts) 
	{
		tree.insert(p);
	}

	return  0;
}

