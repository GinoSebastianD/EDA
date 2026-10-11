#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;


//dimensiones X e Y
constexpr int DIM = 2;

//declara una constante que representa infinito positivo
const double INF = numeric_limits<double>::infinity();

/*
Rect a = { {1, 2}, {5, 7} };
Rect b = { {4, 6}, {8, 9} };

cout << a.lo[0];        // 1: mínimo en X
cout << a.area();       // 20
cout << a.isValid();    // 1: true
cout << a.overlaps(b);  // 1: true
cout << (a == b);       // 0: false
*/


// ---------------------------------------------------------------------------
//  Sección 2: I = (I0, I1, ..., In-1), donde cada Ii es un intervalo
//  cerrado [a, b] que describe la extensión del objeto en la dimensión i.
// ---------------------------------------------------------------------------
struct Rect
{
	//Rect r = {{1, 2}, {5, 7}};
	array<double, DIM> lo; //inferior
	array<double, DIM> hi; //superior

	double area() const {
		double a = 1.0;
		for (int d = 0; d < DIM; d++)
		{
			a = a * (hi[d] - lo[d]);
		}
		return a;
	}

	bool overlaps(const Rect& o) const {
		for (int d = 0 ; d < DIM ; d++){
			if (lo[d] > o.hi[d] ||  o.hi[d] > lo[d]){
				return false;
			}
		}
		return true;
	}

	bool operator ==(const Rect& o) const {
		return (lo == o.lo && hi == o.hi);
	}

};


//bounding box
Rect combine(const Rect& a, const Rect& b) {
	Rect r;
	for (int d = 0 ; d < DIM; d++){
		r.lo[d] = min(a.lo[d], b.lo[d]);
		r.hi[d] = max(a.hi[d], b.hi[d]);
	}
	return r;
}


// Cuánto debe crecer el área de 'r' para incluir a 'add'
double enlargement(const Rect& r, const Rect& add) {
	return (combine(r, add).area() - r.area());
}


// solo se para imprimir esto: [(1,2)-(5,7)]
ostream& operator<<(ostream& os, const Rect& r) {
	os << "[(";
	for (int d = 0; d < DIM; d++) os << r.lo[d] << (d + 1 < DIM ? "," : "");
	os << ")-(";
	for (int d = 0; d < DIM; d++) os << r.hi[d] << (d + 1 < DIM ? "," : "");
	return os << ")]";
}

// ---------------------------------------------------------------------------
//  Sección 2: entradas y nodos
//    - Entrada de hoja:    (I, tuple-identifier)
//    - Entrada interna:    (I, child-pointer)
// ---------------------------------------------------------------------------

struct Node;

//ejemplo
/*Rect caja = {{1, 1}, {4, 3}};
Entry e{caja, nullptr, 42};*/

/*0: hoja.
1: nodo interno cuyos hijos son hojas.
2: nodo interno cuyos hijos están en el nivel 1.*/

struct Entry
{
	Rect I;
	Node* child = nullptr;
	int tuple = -1;
};

struct Node
{
	explicit Node(int lvl) : level(lvl) {}
	int level;
	vector<Entry> entradas;

	bool eshoja() const {
		return level == 0;
	}
};


Rect coveringRect(const Node* n) {
	Rect r = n->entradas[0].I;
	for (const auto& e : n->entradas)
	{
		r = combine(r,e.I);
	}
	return r;
}

Rect coveringRect(const vector<Entry>& g) {
	Rect r = g[0].I;
	for (const auto& e: g)
	{
		r = combine(r, e.I);
	}
	return r;
}

enum class SplitAlgorithm
{
	Exhaustive, Quadratic, Linear
};
// ===========================================================================
//  R-tree
//
//  Propiedades (sección 2):
//   (1) Todo nodo hoja tiene entre m y M registros, salvo que sea la raíz.
//   (2) Cada registro (I, tuple) de una hoja: I es el menor rectángulo que
//       contiene al objeto.
//   (3) Todo nodo interno tiene entre m y M hijos, salvo que sea la raíz.
//   (4) Cada entrada (I, child) de un nodo interno: I es el menor rectángulo
//       que contiene a los rectángulos del nodo hijo.
//   (5) La raíz tiene al menos dos hijos, salvo que sea hoja.
//   (6) Todas las hojas están en el mismo nivel.
// ===========================================================================


class Rtree{

public:
	const int M;
	const int m;
	const SplitAlgorithm alg;
	Node* root;

	Rtree(int maxEntradas, int minEntradas , SplitAlgorithm splitalg) : M(maxEntradas) , m(minEntradas) , alg(splitalg) , root(new Node(0)) {}
	/*if (alg == SplitAlgorithm::Exhaustive && M > 20)
            throw invalid_argument("el algoritmo exhaustivo solo es viable con M pequeño");*/
	
	vector<int> search(const Rect& r) const{
		vector<int> result;
		search(root,r,result);
		return result;
	}
	
	void insert(const Rect& I, int tuple) {


	}




private:
	//3.1 Search
	void search(const Node* T, const Rect& r,  vector<int>& vec) const{
		if (!T->eshoja()) {
			// S1. [Buscar en subárboles] Si T no es hoja, revisar cada
		    //     entrada E para ver si E.I se superpone con S. Para cada
		    //     entrada que se superpone, invocar Search en el subárbol
		    //     cuya raíz es apuntada por E.child.
			for (const auto& e: T->entradas){
				if (e.I.overlaps(r)){
					search(e.child, r, vec);
				}
			}
		}
		else{
			// S2. [Buscar en el nodo hoja] Si T es hoja, revisar todas las
			//     entradas E para ver si E.I se superpone con S. Si es así,
			//     E es un registro que cumple la condición.
			for (const auto& e: T->entradas){
				if (e.I.overlaps(r)){
					vec.push_back(e.tuple);
				}
			}
		}
	}
	

	void insertEntry() {

	}



};



























