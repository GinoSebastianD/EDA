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
	

	void insertEntry(const Entry& E, int level ) {
		vector<Node*> path = chooseleaf(E.I,level);
		Node* L = path.back();
		L->entradas.push_back(E);
		Node* LL = nullptr;
		if ((int)L->entradas.size()> M)
		{
			LL = splitNode(L);
		}

		Node* rootsplit = adjustTree(path, LL);



	}

	vector<Node*> chooseleaf(const Rect I, int level)  const{
		// CL1. [Inicializar] N = raíz.
		Node* N = root;
		vector<Node*> path{ N };

		// CL2. [Verificar hoja] Si N es hoja (o está en el nivel pedido),
	    //      devolver N.
		while (N->level > level)
		{
			// CL3. [Elegir subárbol] Sea F la entrada de N cuyo rectángulo
		   //      F.I necesita crecer menos para incluir I. Los empates se
		   //      resuelven eligiendo la entrada con rectángulo de menor área.
			const Entry* F = nullptr;
			double bestEnl = INF, bestarea = INF;
			for (const auto& e : N->entradas)
			{
				double Enl = enlargement(e.I, I);
				double area = e.I.area();
				if (Enl < bestEnl || (Enl == bestEnl && area < bestarea) )
				{
					F = &e;
					bestEnl = Enl;
					bestarea = area;
				}

			}
			// CL4. [Descender hasta llegar a una hoja] N = F.child;
		   //      repetir desde CL2.
			N = F->child; //baja por aqui
			path.push_back(N); //guardamos por donde baja
		}
		return path;

	}


	Node* splitNode( Node* N) {
		vector<Entry> E = N->entradas;
		vector<Entry> G1, G2;

		switch (alg)
		{
		case SplitAlgorithm::Exhaustive: 
			exaustivesplit(); 
			break;
		case SplitAlgorithm::Quadratic: 
			splitWithSeeds(); 
			break;
		case SplitAlgorithm::Linear:
			splitWithSeeds();
			break;

		default:
			break;
		}



	}
	// ----------------------------------------------------------------------
	//  3.5.1 Exhaustive Algorithm
	//  Generar todas las agrupaciones posibles y elegir la que minimiza el
	//  área total de los dos rectángulos. El número de agrupaciones es
	//  aproximadamente 2^(M-1), por lo que solo es práctico con M pequeño.
	// ----------------------------------------------------------------------

	void exaustivesplit(const vector<Entry>& E, vector<Entry>& G1, vector<Entry>& G2) {




	}
	 
	// ----------------------------------------------------------------------
	 //  3.5.2 Quadratic Split  y  3.5.3 Linear Split
	 //  Ambos comparten los pasos QS1-QS3. Según el paper, el algoritmo
	 //  lineal es idéntico al cuadrático salvo dos cosas:
	 //    - usa LinearPickSeeds en lugar de PickSeeds, y
	 //    - su PickNext elige cualquier entrada restante.
	 //  linear = false -> Quadratic Split;  linear = true -> Linear Split
	 // ----------------------------------------------------------------------
	void splitWithSeeds(vector<Entry> E, vector<Entry>& G1, vector<Entry>& G2 , bool lineal) {
		int s1, s2;
		if (lineal){
			pickseedsLineal(E,s1,s2);
		}
		else{
			pickseeds(E,s1,s2);
		}

		assert(s1 != s2);

		G1.push_back(E[s1]);
		G2.push_back(E[s2]);

		Rect R1 = E[s1].I;
		Rect R2 = E[s2].I;

		//elimino las semillas del vector de entradas
		//primero se elimina el indice mayor
		E.erase(E.begin() + max(s1, s2));
		E.erase(E.begin() + min(s1, s2));

		while (true)
		{
			// QS2. [Verificar si terminó] Si todas las entradas fueron
			//asignadas, detenerse. Si un grupo tiene tan pocas
			//      entradas que todas las restantes deben asignársele para
			//      que llegue al mínimo m, asignarlas y detenerse.
			if (E.empty())
			{
				break;
			}
			if ((int)(G1.size() + E.size()) <= m)
			{
			
				G1.insert(G1.end(), E.begin(), E.end());
				break;
			}
			if ((int)(G2.size() + E.size() <= m))
			{
				G2.insert(G2.end(), E.begin(), E.end());
				break;
			}
		    // QS3. [Seleccionar entrada a asignar] Invocar PickNext para
		   //      elegir la siguiente entrada. Agregarla al grupo cuyo
		   //      rectángulo necesite crecer menos para incluirla. Los
		   //      empates se resuelven agregándola al grupo de menor área,
		   //      luego al de menos entradas, luego a cualquiera.
		   //      Repetir desde QS2.

			int k = lineal ? linearPickNext(E) : picknext(E,R1,R2);

			Entry e = E[k];
			E.erase(E.begin() + k);

			//cuanto crece en cada grupo
			double d1 = enlargement(R1, e.I);
			double d2 = enlargement(R2, e.I);
			bool toG1;

			if (d1!=d2)
			{
				toG1 = d1 < d2;
			}
			else if (R1.area() != R2.area()) //si el crecimiento es igual , vemos el area menor
			{
				toG1 = R1.area() < R2.area(); //elegimos la menor area
			}
			else
			{
				toG1 = G1.size() <= G2.size(); //si no es ninguna de las anteriores vemos el tamaño de las entradas.
			}


			if (toG1)
			{
				G1.push_back(e); //agregamos al grupo
				R1 = combine(R1, e.I); //actualizar bounding box
			}
			else
			{
				G2.push_back(e);
				R2 = combine(R2, e.I);
			}
			


		}



	}


// ----------------------------------------------------------------------
//  PickSeeds (cuadrático): elegir las dos entradas que serán las
//  primeras de cada grupo.
// ----------------------------------------------------------------------
	void pickseeds(const vector<Entry>& E, int&	s1, int& s2) const {
		s1 = 0, s2 = 1;

		double maxwaste = numeric_limits<double>::lowest();
		for (int i = 0; i < (int)E.size(); i++)
		{
			for (int j = i + 1; j < (int)E.size(); j++)
			{
				double waste = combine(E[i].I, E[j].I).area() - E[i].I.area() - E[j].I.area();
				if (waste > maxwaste){
					maxwaste = waste;
					s1 = i;
					s2 = j;
				}

			}
		}
	}
	// ----------------------------------------------------------------------
	//  PickNext (cuadrático): elegir una entrada restante para asignarla.
	// ----------------------------------------------------------------------
	int picknext(const vector<Entry>& E, const Rect& R1 , const Rect& R2) const {
		// PN1. [Determinar el costo de poner cada entrada en cada grupo]
	  //      Para cada entrada E no asignada, calcular d1 = crecimiento
	  //      del rectángulo del grupo 1 para incluir E.I; y d2 igual
	  //      para el grupo 2.
	  // PN2. [Encontrar la entrada con mayor preferencia por un grupo]
	  //      Elegir la entrada con la máxima diferencia entre d1 y d2.
		int best = 0;
		double maxdiff = -INF;
		for (int i = 0 ; i < (int)E.size(); i++)
		{
			double d1 = enlargement(R1, E[i].I);
			double d2 = enlargement(R2, E[i].I);
			double diff = (d1 > d2) ? d1 - d2 : d2 - d1;
			if (diff > maxdiff)
			{
				maxdiff = diff;
				best = i;
			}

		}
		return best;



	}



// ----------------------------------------------------------------------
//  3.5.3 LinearPickSeeds
// ----------------------------------------------------------------------
	void pickseedsLineal(const vector<Entry>& E, int& s1, int& s2) const {
		s1 = 0, s2 = 1;
		int n = (int)E.size();
		double bestSep = numeric_limits<double>::lowest();

		for (int d = 0 ; d < DIM; d++){
		 // LPS1. [Encontrar rectángulos extremos en todas las dimensiones]
		 //       En cada dimensión, encontrar la entrada cuyo rectángulo
		 //       tiene el lado inferior más alto y la que tiene el lado
		 //       superior más bajo. Registrar la separación.
			int highestlow = 0; //mayor límite inferior
			for (int i = 1 ; i < n ; i++)
			{
				/* A: [0, 2]	B: [1, 3] C: [6, 8] D: [7, 9] E: [4, 5]
				Los límites inferiores son 0, 1, 6, 7, 4. El mayor es 7, correspondiente a D. */
				if (E[i].I.lo[d] > E[highestlow].I.lo[d]){
					highestlow = i;
				}
			}
			//menor límite superior
			int loweshigh = (highestlow == 0) ? 1 : 0;

			for (int i = 0; i < n; i++)
			{
				if (i != highestlow && (E[i].I.hi[d]  < E[loweshigh].I.hi[d]))
				{
					loweshigh = i;
				}
			}
			//mayor límite inferior − menor límite superior de otra entrada
			double separacion = E[highestlow].I.lo[d] - E[loweshigh].I.hi[d];


			// LPS2. [Ajustar por la forma del conjunto de rectángulos]
			//Normalizar las separaciones dividiéndolas por el ancho
			//del conjunto completo en la dimensión correspondiente.

			double minLO = INF;
			double maxHI = -INF;

			for (const auto& e: E)
			{
				minLO = min(minLO, e.I.lo[d]);
				maxHI = max(maxHI, e.I.hi[d]);
			}

			double width = maxHI - minLO;
			double normalizacion = (width > 0) ? separacion / width : 0.0;

			// LPS3. [Seleccionar el par más extremo] Elegir el par con la
		    // mayor separación normalizada en cualquier dimensión.

			if (normalizacion > bestSep)
			{
				bestSep = normalizacion;
				s1 = highestlow;
				s2 = loweshigh;
			}

		}



	}
	// 3.5.3: en el algoritmo lineal, PickNext simplemente elige cualquiera
  // de las entradas restantes.
	int linearPickNext(const vector<Entry>&) const { return 0; }




};



























