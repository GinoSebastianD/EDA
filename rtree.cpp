#include "iostream"
#include "vector"
#include "limits"
#include "algorithm"

using namespace std;

struct Rect
{
	double maxX;
	double minX;
	double maxY;
	double minY;

	Rect(double maxX = 0, double minX = 0 , double maxY = 0 , double minY = 0) : maxX(maxX) , minX(minX) , maxY(maxY) , minY(minY) {}

	bool overlaps(const Rect& other) const{	
		return !(minX > other.maxX || maxX < other.minX
			|| minY > other.maxY || maxY < other.minY);
	}

	bool contains(const Rect& other) const{
		return (minX <= other.minX  && minY <= other.minY &&
				maxX >= other.maxX  && maxY >= other.maxY );
	}

	Rect combine(const Rect& other) const{
		return Rect(min(minX, other.minX), min(minX, other.minX),
			max(maxX, other.maxX), max(maxY, other.maxY));
	}

	double area() const {
		return (maxX - minX) * (maxY - minY);
	}
	bool equals(const Rect& other) const {
		return (minX == other.minX && minY == other.minY && maxX == other.maxX && maxY == other.maxY);
	}

};

const int M_maximo = 5;
const int m_minimo = 2;

struct Node;


struct Entry
{
	Rect rect;
	Node* child;
	Entry() : rect(Rect()), child(nullptr) {}
	Entry(const Rect& r , Node* c = nullptr): rect(r) , child(c) {}
};


struct Node{
	bool eshoja;
	vector<Entry> entradas;
	Node(bool esH = true): eshoja(esH) {}
};


class RTree
{
private:

	int chooseleaf(Node* node, const Rect& rect) {
		double minAmpliacion = numeric_limits<double>::max();
		double minArea = numeric_limits<double>::max();
		int idx;
		for (int i = 0; i < (int)node->entradas.size(); i++) {
			Rect combi = node->entradas[i].rect.combine(rect);
			double ampliacion = combi.area() - node->entradas[i].rect.area();
			double area = node->entradas[i].rect.area();

			if (ampliacion < minAmpliacion || (ampliacion == minAmpliacion && area < minArea)) {
				minAmpliacion = ampliacion;
				minArea = area;
				idx = i;
			}

		}
		return idx;
	}

	void adjustTree(vector<Node*>& path , vector<int> indices , Node* child1, Node* child2) {
		int i = path.size() - 1;
		while (i >= 0) {
			Node* parent = path[i];
			int idx = indices[i];

			parent->entradas[idx].rect = boundingRect(parent->entradas[idx].child->entradas);

			if (child2) {
				parent->entradas.push_back(Entry(boundingRect(child2->entradas),child2) );
				if ((int)parent->entradas.size() > M_maximo) {
					Node* nuevo_hermano = splitNode(parent);
					child2 = nuevo_hermano;
				}
				else {
					child2 = nullptr;
				}
			}
			i--;

		}
		if (child2) {
			
			Node* oldroot = root;
			root = new Node(false);
			root->entradas.push_back(Entry(boundingRect(oldroot->entradas), oldroot));
			root->entradas.push_back(Entry(boundingRect(child2->entradas), child2));

		}

	}


	Rect boundingRect(const vector<Entry>& entradas) {
		double mnX = numeric_limits<double>::max();
		double mxX = numeric_limits<double>::lowest();
		double mnY = numeric_limits<double>::max();
		double mxY = numeric_limits<double>::lowest();

		for (int i = 0; i < (int)entradas.size(); i++) {
			mnX = min(mnX, entradas[i].rect.minX);
			mxX = max(mxX, entradas[i].rect.maxX);
			mnY = min(mnY, entradas[i].rect.minY);
			mxY = max(mxY, entradas[i].rect.maxY);
		}
		return Rect(mnX, mnY, mxX, mxY);
	}

	Rect mbrNode(Node* node) {
		return boundingRect(node->entradas);
	}


	Node* splitNode(Node* node) {
		return splitNodeCuadratico(node);
	}
	//pickNextLineal es lo mismo para lineal y cuadratic
	void pickNextLineal( vector<Entry>& entradas , vector<bool>& assigned , pair<Node*,Node*>& grupos, int& count) {
		return pickNextCuadratico(entradas, assigned, grupos, count);


	}


	/////////////////////////////////////////////////////
	//Lineal
	/////////////////////////////////////////////////////
	
	pair<int, int> pickseedsLineal(Node* node) {

		double maxminX = numeric_limits<double>::lowest();
		double minmaxX = numeric_limits<double>::max();
		double maxminY = numeric_limits<double>::lowest();
		double minmaxY = numeric_limits<double>::max();

		double globalmaxX = numeric_limits<double>::lowest();
		double globalminX = numeric_limits<double>::max();
		double globalmaxY = numeric_limits<double>::lowest();
		double globalminY = numeric_limits<double>::max();

		int idxMaxMinX = 0;
		int idxMinMaxX = 0;
		int idxMaxMinY = 0;
		int idxMinMaxY = 0;


		for (int i = 0; i < (int)node->entradas.size(); i++) {
			const Rect& rec = node->entradas[i].rect;
			globalmaxX = max(globalmaxX, rec.maxX);
			globalminX = min(globalminX, rec.minX);
			globalmaxY = max(globalmaxY, rec.maxX);
			globalminY = min(globalminY, rec.minY);

			if (rec.minX > maxminX) {
				maxminX = rec.minX;
				idxMaxMinX = i;
			}
			if (rec.maxX < minmaxX){
				minmaxX = rec.maxX;
				idxMinMaxX = i;
			}
			if (rec.minY > maxminY) {
				maxminY = rec.minY;
				idxMaxMinY = i;
			}
			if (rec.maxY < minmaxY) {
				minmaxY = rec.maxY;
				idxMinMaxY = i;
			}
		}

		if (idxMaxMinX == idxMinMaxX && idxMaxMinY == idxMinMaxY) {
			return make_pair(0, 1);
		}

		//calculamos la separacion en ambas direcciones

		double sepX = max(0.0, maxminX - minmaxX) / max(1.0, globalmaxX - globalminX);
		double sepY = max(0.0, maxminY - minmaxY) / max(1.0, globalmaxY - globalminY);
		

		return (sepX >= sepY) ? make_pair(maxminX, minmaxX) : make_pair(maxminY, minmaxY);

	}






	Node* splitNodeLineal(Node* node) {
		pair<int, int> seeds = pickseedsLineal(node);
		int iA = seeds.first;
		int iB = seeds.second;
	
		Node* g1 = new Node(node->eshoja);
		g1->entradas.push_back(node->entradas[iA]);
		Node* g2 = new Node(node->eshoja);
		g2->entradas.push_back(node->entradas[iB]);

		pair<Node*, Node*> grupos = make_pair(g1, g2);
		int total = (int)node->entradas.size();
		vector<bool> assigned(total, false);
		assigned[iA] = true;
		assigned[iB] = true;
		int count = 2;

		while (count < total) {
			int rem = total - count;
			if ((int)g1->entradas.size() + rem == m_minimo) {
				for (int i = 0; i < total; i++) {
					if (!assigned[i]) {
						g1->entradas.push_back(node->entradas[i]);
						assigned[i] = true;
						count++;
					}
				}
				break;
			}
			if ((int)g2->entradas.size() + rem == m_minimo) {
				for (int i = 0; i < total; i++) {
					if (!assigned[i]) {
						g2->entradas.push_back(node->entradas[i]);
						assigned[i] = true;
						count;
					}

				}
				break;
			}

			pickNextLineal(node->entradas, assigned, grupos, count);
		}
		node->entradas = g1->entradas;
		return g2;

	}








	/////////////////////////////////////////////////////
	//cuadratico
	/////////////////////////////////////////////////////

	pair<int, int> pickSeedsCuadratico(Node* node) {
		double maxWaste = numeric_limits<double>::lowest();
		int bestI;
		int bestJ;
		for (int i = 0; i < (int)node->entradas.size(); i++) {
			for (int j = i + 1; j < (int)node->entradas.size(); j++) {
				double waste = node->entradas[i].rect.combine(node->entradas[j].rect).area() -
					node->entradas[i].rect.area() - node->entradas[j].rect.area();
				if (waste > maxWaste) {
					maxWaste = waste;
					bestI = i;
					bestJ = j;
				}

			}
		}
		return make_pair(bestI, bestJ);
	}

	void pickNextCuadratico(vector<Entry>& entradas, vector<bool>& assigned, pair<Node*,Node*>grupos, int& count) {
		double maxDiff = numeric_limits<double>::lowest();
		int sel = -1;
		Node* target = nullptr;

		for (int i = 0; i < (int)entradas.size(); i++) {
			if (assigned[i]) {
				continue;
			}
			
			double d1 = mbrNode(grupos.first).combine(entradas[i].rect).area() -
				mbrNode(grupos.first).area();
			double d2 = mbrNode(grupos.second).combine(entradas[i].rect).area() -
				mbrNode(grupos.second).area();
			double diff = abs(d1 - d2);

			if (diff > maxDiff) {
				maxDiff = diff;
				sel = i;
				if (d1 < d2) {
					target = grupos.first;
				}
				else if (d2 < d1) {
					target = grupos.second;
				}
				else {
					target = (grupos.first->entradas.size() <= grupos.second->entradas.size()) ? grupos.first : grupos.second;
				}
			}


		}
		
		if (sel != -1) {
			assigned[sel] = true;
			target->entradas.push_back(entradas[sel]);
			count++;
		}

	}



	Node* splitNodeCuadratico(Node* node) {
		pair<int, int> semillas = pickSeedsCuadratico(node);
		int iA = semillas.first;
		int iB = semillas.second;

		Node* g1 = new Node(node->eshoja);
		g1->entradas.push_back(node->entradas[iA]);
		Node* g2 = new Node(node->eshoja);
		g2->entradas.push_back(node->entradas[iB]);

		pair<Node*, Node*> grupos = make_pair(g1, g2);
		
		int total = (int)node->entradas.size();
		
		vector<bool> assigned(total,false);
		assigned[iA] = true;
		assigned[iB] = true;
		int cont = 2;

		while(cont < total){
			int rem = total - cont;
			if ((int)g1->entradas.size() + rem == m_minimo) {
				for (int i = 0; i < total; i++) {
					if (!assigned[i]) {
						g1->entradas.push_back(node->entradas[i]);
						assigned[i] = true;
						cont++;
					}
				}
				break;
			}
			if ((int)g2->entradas.size() + rem == m_minimo) {
				for (int i = 0; i < total; i++) {
					if (!assigned[i]) {
						g2->entradas.push_back(node->entradas[i]);
						assigned[i] = true;
						cont++;
					}
				}
				break;
			}	
			pickNextCuadratico(node->entradas, assigned, grupos, cont);
		}
		node->entradas = g1->entradas;
		return g2;

	}



public:
	Node* root;
	RTree() { root = new Node(true); }

	void insert(const Rect& rect) {
		Node* node = root;
		vector<Node*> path;
		vector<int> indices;

		while (!node->eshoja) {
			int idx = chooseleaf(node, rect);
			path.push_back(node);
			indices.push_back(idx);
			node = node->entradas[idx].child;
		}

		//llego a la hoja , y ya tenemos a node apuntando al indice donde debe ir
		node->entradas.push_back(Entry(rect));
		if ((int)node->entradas.size() > M_maximo) {
			Node* newSibling = splitNode(node);
			adjustTree();
		}
		else {

		}

	}



};






int main() {
	return 0;
}
