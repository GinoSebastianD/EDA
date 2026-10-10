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

struct Entry
{

};




























