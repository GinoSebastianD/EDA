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

    int chooseSubtree(node* node, const Rect& rect) {
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

    node* splitNode(node* node){
        return
    }

    Rect boundingRect(const vector<Entry>& es){
        double mnX = numeric_limits<double>::max();  //Durante el recorrido, guardarán el menor minX y el menor minY.
        double mnY = numeric_limits<double>::max();
        double mxX = numeric_limits<double>::lowest(); //Guardarán el mayor maxX y el mayor maxY.
        double mxX = numeric_limits<double>::lowest();
        for(int i = 0; i < (int)es.size() ; i++){
            mnX = min(mnX , es[i].rect.minX);
            mnY = min(mnY , es[i].rect.minY);
            mxX = max(mxX , es[i].rect.maxX);
            mxY = max(mxY , es[i].rect.maxY);
        }
        return Rect(mnX,mnY,mxX,mnY);

    }

    Rect mbrNode(node* node){
        return boundingRect(node->entradas);
    }


    //
    //  SPLIT CUADRATICO
    //


    void pickNextCuadratico(vector<Entry>& entradas, vector<bool>& assigned , pair<node*,node*>& grupos, int& counts){
        double maxDiff = numeric_limits<double>::lowest(); //guardará la mayor diferencia encontrada entre los costos de colocar una entrada en g1 o en g2.
        int sel = -1; //guardara el indice de la entrada elegida.
        node* target = nullptr; //apuntara al grupo donde debe ir esa entrada.

        for(int i = 0; i < (int)entradas.size() ; i++){
            if(assigned[i]){ //Recorre todas las entradas. Si la entrada i ya fue asignada, continue salta a la siguiente.
                continue;
            }
            double d1 = mbrNode(grupos.first).combine(entradas[i].rect).area() -
                        mbrNode(grupos.first).area();
            double d2 = mbrNode(grupos.second).combine(entradas[i].rect).area() -
                        mbrNode(grupos.second).area();
            double diff = abs(d1-d2);
            
            if(diff > maxDiff){
                maxDiff = diff;
                sel = i;
                if(d1 < d2){
                    target = grupos.first;
                }
                else if(d2 < d1){
                    target = grupos.second;
                }
                else{ //Si d1 == d2, va al grupo que tenga menos entradas.
                    (grupos.first->entradas.size() <= grupos.second->entradas.size()) ? grupos.first : grupos.second;
                }
                //Termina de examinar las entradas pendientes. sel contiene la 
                //que tuvo la mayor diferencia de costos, y target indica su grupo.
            
            }
            





        }



    }


    void pickNextLineal(vector<Entry>& entradas, vector<bool>& assigned , pair<node* , node*>& grupos, int& counts){
        pickNextCuadratico(entradas, assigned, grupos, counts);

    }


    //
    //  SPLIT LINEAL
    //

    pair<int,int> pickseedslineal(node* node){
        //bordes

        double maxMinX = numeric_limits<double>::lowest();
        double minMaxX = numeric_limits<double>::max();
        double maxMinY = numeric_limits<double>::lowest();
        double minMaxY = numeric_limits<double>::max;
        //bordes globales
        double globalMinX = numeric_limits<double>::lowest();
        double globalMaxX = numeric_limits<double>::max();
        double globalMinY = numeric_limits<double>::lowest();
        double globalMaxY = numeric_limits<double>::max();

        //guardaremos que entrada se produjo
        int idxmaxMinX = 0 , idxminMaxX = 0;
        int idxmaxMinY = 0 , idxminMaxY = 0;

        for(int i = 0 ; i < (int)node->entradas.size() ; i++){
            const Rect& r = node->entradas[i].rect;

            globalMaxX = max(globalMaxX , r.maxX);
            globalMinX = min(globalMinX , r.minX);
            globalMaxY = max(globalMaxY , r.maxY);
            globalMinY = min(globalMinY , r.minY);

            if(r.minX > maxMinX){
                maxMinX = r.minX;
                idxmaxMinX = i;
            }
            if(r.maxX < minMaxX){
                minMaxX = r.maxX;
                idxminMaxX = i;
            }
            if(r.minY > maxMinY){
                maxMinY = r.minY;
                idxmaxMinY = i;
            }
            if(r.maxY < minMaxY){
                minMaxY = r.maxY;
                idxminMaxY = i;
            }
        }

        if(idxmaxMinX == idxminMaxX && idxmaxMinY == idxminMaxY){
            return make_pair(0,1);
        }

        double sepX = max(0.0 , maxMinX - minMaxX ) / max(1.0 ,globalMaxX - globalMinX ) ;
        double sepY = max(0.0 , maxMinY - minMaxX ) / max(1.0 ,globalMaxY - globalMinY );

        return (sepX >= sepY) ? make_pair(idxmaxMinX ,  idxminMaxX ) : make_pair(idxmaxMinY , idxminMaxY );
    }





    Node* splitNodeLineal(node* node){

        pair<int,int> seeds = pickseedslineal(node); // examina las entradas de node y devuelve dos indices. esas entradas seran las semillas: una iniciara el primer grupo y la otra el segundo grupo
        int iA = seeds.first;
        int iB = seeds.second;

        node* g1 = new node(node->eshoja);
        g1->entradas.push_back(node->entradas[iA]);
        node* g2 = new node(node->eshoja);
        g2->entradas.push_back(node->entradas[iB]);

        pair<node*,node*> grupos = make_pair(g1,g2);
        int total = (int)node->entradas.size();
        vector<bool> assigned(total,false);

        assigned[iA] = true;
        assigned[iB] = true;

        int cont = 2;
        while(cont < total){
            int rem = total - cont;
            //Por ejemplo: g1 tiene 1 entrada y solo queda 1 sin asignar. 1 + 1 == 2, así que esa entrada debe ir a g1.
            if((int)g1->entradas.size() + rem == m_minimo){ //Comprueba si g1 solo podrá alcanzar el mínimo permitido recibiendo todas las entradas restantes.
                for(int i = 0; i < total ; i++){
                    if(!assigned[i]){ //Actúa únicamente sobre las que aún no se asignaron.
                        //Añade la entrada a g1, la marca como asignada y aumenta el contador.
                        g1->entradas.push_back(node->entradas[i]);
                        assigned[i] = true;
                        cont++;
                    }
                }
                break;
            }
            if((int)g2->entradas.size() + rem == m_minimo){ //Comprueba si g1 solo podrá alcanzar el mínimo permitido recibiendo todas las entradas restantes.
                for(int i = 0; i < total ; i++){
                    if(!assigned[i]){ //Actúa únicamente sobre las que aún no se asignaron.
                        //Añade la entrada a g1, la marca como asignada y aumenta el contador.
                        g2->entradas.push_back(node->entradas[i]);
                        assigned[i] = true;
                        cont++;
                    }
                }
                break;
            }



        }


    }



//    void adjustTree(vector<Node*>& path , vector<int>& indices, Node* hijo1 , Node* hijo2 ){



 //   }

public:
    Rtree(){
        root = new node(true);
    }

    void insert(const Rect& rect){
        node* node = root;
        vector<node*> path;
        vector<int> indices;
        while(!node->eshoja){
            int idx = chooseSubtree(node , rect);
            path.push_back(node);
            indices.push_back(idx);
            node = node->entradas[idx].child;
        }
        //en caso sea una hoja
        node->entradas.push_back(Entry(rect));

        if((int)node->entradas.size() > M_maximo){
            Node* nuevohermano = splitNode(node);
        }
        else{

        }




    }



};


int main(){

    return 0;


}
