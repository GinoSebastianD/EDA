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

	Rect(double maxX = 0, double minX = 0, double maxY = 0, double minY = 0) : maxX(maxX), minX(minX), maxY(maxY), minY(minY) {}

	bool overlaps(const Rect& other) const { // si hay sobreposicion
		return !(minX > other.maxX || maxX < other.minX
			|| minY > other.maxY || maxY < other.minY);
	}

	bool contains(const Rect& other) const {
		return (minX <= other.minX && minY <= other.minY &&
			maxX >= other.maxX && maxY >= other.maxY);
	}

	Rect combine(const Rect& other) const {
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
	Entry(const Rect& r, Node* c = nullptr) : rect(r), child(c) {}
};


struct Node {
	bool eshoja;
	vector<Entry> entradas;
	Node(bool esH = true) : eshoja(esH) {}
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

	void adjustTree(vector<Node*>& path, vector<int> indices, Node* child1, Node* child2) {
		int i = path.size() - 1;
		while (i >= 0) {
			Node* parent = path[i];
			int idx = indices[i];

			parent->entradas[idx].rect = boundingRect(parent->entradas[idx].child->entradas);

			if (child2) {
				parent->entradas.push_back(Entry(boundingRect(child2->entradas), child2));
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
	void pickNextLineal(vector<Entry>& entradas, vector<bool>& assigned, pair<Node*, Node*>& grupos, int& count) {
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
			if (rec.maxX < minmaxX) {
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

	void pickNextCuadratico(vector<Entry>& entradas, vector<bool>& assigned, pair<Node*, Node*>grupos, int& count) {
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

		vector<bool> assigned(total, false);
		assigned[iA] = true;
		assigned[iB] = true;
		int cont = 2;

		while (cont < total) {
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


	//delete fun

	Node* findleaf(Node* node, const Rect& rect, vector<Node*>& path) {
		if (!node->eshoja) { //cuando el nodo es interno

			for (int i = 0 ; i < (int)node->entradas.size(); i++)
			{
				if (node->entradas[i].rect.overlaps(rect) ) {
					path.push_back(node);
					Node* result = findleaf(node->entradas[i].child , rect, path);
					if (result != nullptr) //Si la búsqueda encontró una hoja, la devuelve inmediatamente. path conserva los nodos recorridos hasta ella.
					{
						return result;
					}
					path.pop_back();
				}

			}
			return nullptr;

		}
		//cuando node es una hoja
		for (int i = 0; i < (int)node->entradas.size(); i++)
		{
			//Compara cada rectángulo de la hoja con el buscado. Si encuentra uno con las mismas cuatro coordenadas, devuelve el nodo hoja,
			// no la posición de la entrada. Por eso deleteEntry después vuelve a
			// recorrer esa hoja para localizar y borrar la entrada.
			if (node->entradas[i].rect.equals(rect)) {
				return node;
			}
		}
		//Si ninguna entrada de la hoja es igual, indica que allí no está.
		return nullptr;
	}

	//condense Tree
	
	void condenseTree(Node* leaf, vector<Node*>& path) {
		//N empieza en la hoja donde se borró la entrada. 
		// Q guardará los nodos retirados para procesar sus entradas al final.
		Node* N = leaf;
		vector<Node*> Q;

		while (!path.empty()) {
			//Toma el padre más cercano a N y lo quita de path. Así recorre el camino de abajo hacia arriba.
			Node* parent = path.back();
			path.pop_back();

			int idxparent = -1;
			for (int i = 0 ; i < (int)parent->entradas.size(); i++)
			{
				//Busca en el padre la entrada cuyo puntero child apunta exactamente a N. idxInParent será su posición;
				if (parent->entradas[i].child == N)
				{
					idxparent = i;
					break;
				}
			}

			if (idxparent == -1) {
				N = parent;
				continue;
			}

			if ((int)N->entradas.size() < m_minimo) {
				parent->entradas.erase(parent->entradas.begin() + idxparent);
				Q.push_back(N);
			}
			else
			{
				parent->entradas[idxparent].rect = boundingRect(N->entradas);
			}
			//Ahora el padre pasa a ser el nodo examinado
			N = parent;
		}

		for (int i = 0; i < (int)Q.size() ; i++)
		{
			Node* qNode = Q[i];

			for (int j = 0; j < (int)qNode->entradas.size(); j++)
			{
				insert(qNode->entradas[j].rect);
			}

			delete qNode;
		}



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
			adjustTree(path,indices,node,newSibling );
		}
		else {
			adjustTree(path,indices,node,nullptr);
		}

	}

	void deleteEntry(const Rect& rec) {

		//Crea path y busca desde la raíz la hoja que contiene el rectángulo. 
		// findLeaf devuelve esa hoja y deja en path los nodos internos del camino hasta ella.
		vector<Node*> path;
		Node* leaf = findleaf(root, rec, path);

		if (leaf == nullptr) {
			cout << "no se encontro\n";
			return;
		}

		for (int i = 0; i< (int)leaf->entradas.size(); i++)
		{
			if (leaf->entradas[i].rect.equals(rec)) {
				leaf->entradas.erase(leaf->entradas.begin() + i); // eliminar ese rectangulo
				break;
			}
		}
		//Revisa el camino de vuelta hacia la raíz. Actualiza los MBR de los nodos que permanecen y 
		// trata los nodos que quedaron con menos de MIN_ENTRIES entradas
		condenseTree(leaf, path);

		if (!root->eshoja && (int)root->entradas.size() == 1) {
			Node* oldroot = root;
			root = root->entradas[0].child;
			delete oldroot;
		}

	}

};






int main() {
	return 0;
}
