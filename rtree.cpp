#include "bits/stdc++.h"



using namespace std;

struct Rect{
    double minX;
    double minY;
    double maxX;
    double maxY;

    Rect(double minX = 0 , double minY = 0 , double maxX = 0 , double maxY = 0):
        minX(minX) ,minY(minY) , maxX(maxX) , maxY(maxY) {}

    bool superposicion(const Rect& other) const  {  //verificamos si hay superposicion
        return !(minX > other.maxX || maxX < other.minX || //preguntamos si esta a la izquierda o derecha
                 minY > other.maxY || maxY < other.minY ) ; //preguntamos si esta a arriba o abajo
    }  //el "!" cambia el bool, si se cumple alguno de las condiciones devuelve q no hay superposicion caso contrario devuelve que si

    bool contine(const Rect& other) const { // hacemos lo mismo pero comparando si esta dentro el rectangulo de otro rectangulo
        return ( minX <= other.minX && maxX >= other.maxX ||   //comparamos si esta izquierda o derecha
                 minY <= other.minY && maxY >= other.maxY   ); //comparamos arriba o abajo
    }

    Rect combine(const Rect& other) const { // genera un rectangulo mas grande que envuelve a dos , devuelve un rectangulo
        return Rect( min(minX,other.minX) , min(minY,other.minY) , max(maxX,other.maxX) , max(maxY, other.maxY));
    }

    double area() const{
        return (maxX - minX) * (maxY - minY); //area de un rectangulo
    }
    bool equals(const Rect& other) const{
        return (minX == other.minX && minY == other.minY && maxX == maxY && maxY == other.maxY); // verificamos si dos rectangulos son iguales
    }

};

struct node;

struct Entry {
    Rect rect;
    node* child;
    int id;
    Entry(): rect(Rect()) , child(nullptr)  {} //rect guarda un rectangulo y puntero chill inicializamos
    Entry( const Rect& r, node* c = nullptr, int dato = -1 ):rect(r) , child(c) , id(dato){}
};

struct node{
    bool eshoja;
    int level;
    vector<Entry> entradas;
    node(bool l = true ): eshoja(l){};
};

const int M_maximo = 4;
const int m_minimo = 2;


class Rtree{
private:
    node* root;
    int metodo;
    int nextID;

    int chooseleaf(node* node , Rect& rect ){
        double minampliacion = numeric_limits<double>::max();
        double minArea = numeric_limits<double>::max();
        int bestIdx = 0;
        for(int i = 0 ; i < (int)node->entradas.size() ; i++){
            Rect combined = node->entradas[i].rect.combine(rect); //combinamos el rectangulo actual , con el que queremos ingresar
            double ampliacion = combined.area() - node->entradas[i].rect.area(); // calculamos que tanto estamos crediendo en ampliacion con el area
            double area = node->entradas[i].rect.area();

            if(ampliacion < minampliacion || (ampliacion == minampliacion && area < minArea)){ // si la ampliacion es menor entro, O si la ampliacion
                minampliacion = ampliacion; //elegirmos la menor ampliacion                         //es igual y el area es menor , entra
                minArea = area; //escogemos la minima area
                bestIdx = i; // actualizamos el indice
            }
        }
        return bestIdx; //devolbemos el mejor indice
    }

    int chooseSubtree(node* node, const Rect& rect) { //esta no calcula el area xd
        double minEnlargement = numeric_limits<double>::max();
        int bestIdx = 0;
        for (int i = 0; i < node->entradas.size(); i++) {
            Rect combined = node->entradas[i].rect.combine(rect);
            double enlargement = combined.area() - node->entradas[i].rect.area();
            if (enlargement < minEnlargement) {
                minEnlargement = enlargement;
                bestIdx = i;
            }
        }
        return bestIdx;
    }


    
public:
    Rtree() { root = new Node(true); }
        
    void insert( const Rect& rect){
    
    
    }



//    void adjustTree(vector<Node*>& path , vector<int>& indices, Node* hijo1 , Node* hijo2 ){



 //   }

 
 
 
};


int main(){

    return 0;


}
