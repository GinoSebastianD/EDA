#include "iostream"
#include "vector"
#include "algorithm"
#include "fstream"
#include "string"
#include "iomanip"
#include <bits/stdc++.h>

using namespace std;
//solo creo q falta poda XD
struct Point {
    double x;
    double y;
    double z;
    Point(double a = 0 , double b = 0 , double c = 0 ){
        x = a;
        y = b;
        z = c;
    }

};
void mostrarpunto(const Point& p){
    cout << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}
double dist_cuadrada(const Point& a, const Point& b){
    double x = pow(a.x-b.x,2);
    double y = pow(a.y-b.y,2);
    double z = pow(a.z-b.z,2);

    return x + y + z;

}


class Octree{
public:
    Octree* hijos[8];
    vector<Point> puntos;
    Point esquina_inferior_izquierda;
    double h;
    int capacidad;
    int nivel;
    int numero_puntos;



    Octree(Point esquina = Point(0,0,0) , double lado = 128 , int N = 100 , int profundidad = 0){
        esquina_inferior_izquierda = esquina;
        h = lado;
        capacidad = N;
        nivel = profundidad;
        numero_puntos =0;
        for(int i = 0 ; i < 8 ; i++){
            hijos[i] = nullptr;
        }
    }

    bool eshoja() const{
        return hijos[0] == nullptr;
    }


    bool contiene(const Point& p) const{
        return p.x >= esquina_inferior_izquierda.x && p.x <= esquina_inferior_izquierda.x + h &&
               p.y >= esquina_inferior_izquierda.y && p.y <= esquina_inferior_izquierda.y + h &&
               p.z >= esquina_inferior_izquierda.z && p.z <= esquina_inferior_izquierda.z + h;
    }
    int elegirhijo(const Point& p) const{
        int hijo = 0;
        if(p.x >= esquina_inferior_izquierda.x + h / 2){
            hijo = hijo + 1;
        }
        if(p.y >= esquina_inferior_izquierda.y + h / 2){
            hijo = hijo + 2;
        }
        if(p.z >= esquina_inferior_izquierda.z + h / 2){
            hijo = hijo + 4;
        }
        return hijo;

    }

    bool exist(const Point& p) const{
        if(!contiene(p)){
            return false;
        }
        if(!eshoja()){
            double mitad = h/2;
            int hijo = 0;
            if(p.x >= esquina_inferior_izquierda.x + mitad ){
                hijo = hijo + 1;
            }
            if(p.y >= esquina_inferior_izquierda.y + mitad){
                hijo = hijo + 2;
            }
            if(p.z >= esquina_inferior_izquierda.z + mitad){
                hijo = hijo + 4;
            }
            return hijos[hijo]->exist(p);
        }
        //cada uno de los puntos que tiene dentro lo comparamos con el que queremos
        //agregar para ver si existe o no

        for(auto i = 0; i < puntos.size(); i++){
            if(puntos[i].x == p.x && puntos[i].y == p.y && puntos[i].z == p.z){
                return true;
            }
        }
        return false;

    }
    void dividir(){
        double ha = h/2;
        Point b = esquina_inferior_izquierda;

       Point offsets[8] = {
            {b.x,b.y,b.z},
            {b.x + ha, b.y,  b.z},
            {b.x,b.y + ha, b.z},
            {b.x + ha, b.y + ha, b.z},
            {b.x, b.y, b.z + ha},
            {b.x + ha, b.y, b.z + ha},
            {b.x, b.y + ha, b.z + ha},
            {b.x + ha, b.y + ha, b.z + ha},
       };

        for(int i = 0 ; i< 8 ; i++){
            hijos[i] = new Octree(offsets[i], ha , capacidad , nivel+ 1);
        }

        Point medio(b.x + ha , b.y +ha , b.z + ha);

        for(size_t i = 0; i < puntos.size() ; i++){
            Point actual = puntos[i];
            bool derecha = actual.x >= medio.x;
            bool izquierda = actual.y >= medio.y;
            bool atras = actual.z >= medio.z;
            int indice = 0;

            if(derecha){
                indice = indice + 1;
            }
            if(izquierda){
                indice = indice + 2;
            }
            if(atras){
                indice = indice + 4;
            }

            hijos[indice]->insert(actual);

        }
        puntos.clear();
    }

    bool insert(const Point& p ){
        if(eshoja()){
            if(exist(p)){
                return false;
            }
            if(puntos.size() < static_cast<size_t>(capacidad) ){
                puntos.push_back(p);
                numero_puntos++;
                return true;
            }
            dividir();
        }
        double mitad = h /2;
        int hijo = 0;
        if(p.x >= esquina_inferior_izquierda.x + mitad){
            hijo = hijo + 1;
        }
        if(p.y >= esquina_inferior_izquierda.y + mitad){
            hijo = hijo + 2;
        }
        if(p.z >= esquina_inferior_izquierda.z+ mitad){
            hijo = hijo + 4;
        }
        bool insertado = hijos[hijo]->insert(p);
        if (insertado){
            numero_puntos++;
        }
        return insertado;


    }
    double dist_min_a_caja(const Point& p, const Point& esquina, double h) const{
        double dx = 0, dy = 0, dz = 0;

        if(p.x < esquina.x) dx = esquina.x - p.x;
        else if(p.x > esquina.x + h) dx = p.x - (esquina.x + h);

        if(p.y < esquina.y) dy = esquina.y - p.y;
        else if(p.y > esquina.y + h) dy = p.y - (esquina.y + h);

        if(p.z < esquina.z) dz = esquina.z - p.z;
        else if(p.z > esquina.z + h) dz = p.z - (esquina.z + h);

        return dx*dx + dy*dy + dz*dz;
    }

    void buscar(const Point & p , double& distancia , Point& resultado, bool& encontrado, double& ladodelahoja) const{
        if(dist_min_a_caja(p, esquina_inferior_izquierda, h) > distancia){
            return;
        }
        if(eshoja()){
            for(size_t i = 0 ; i < puntos.size() ; i ++){
                double d = dist_cuadrada(p,puntos[i]);
                if(d <= distancia && (!encontrado || d < distancia) ){
                    distancia = d;
                    resultado = puntos[i];
                    ladodelahoja = h;
                    encontrado = true;
                }
            }
        }
        else{
            for(int i = 0; i < 8 ; i++){
                hijos[i]->buscar(p, distancia, resultado, encontrado, ladodelahoja);
            }

        }


    }


    bool find_closet(const Point& p , double radio , Point& result , double& ladohoja) const{
        ladohoja = 0;
        double distancia = pow(radio,2);
        bool encontrad= false;
        buscar(p, distancia , result , encontrad , ladohoja  );
        return encontrad;

    }
    void m_raiz()const{
        cout << "bottomLeft = ";
        mostrarpunto(esquina_inferior_izquierda);
        cout << "\nh = " << h << "\nN = " << capacidad;
        cout << "\nPuntos guardados = " << numero_puntos << "\n";
    }

    void imprimir(int indent = 0) const{
        string sangria(indent * 2, ' ');
        cout << sangria << "Nivel " << nivel << " | esquina=";
        mostrarpunto(esquina_inferior_izquierda);
        cout << " | h=" << h;

        if(eshoja()){
            cout << " hoja puntos=" << puntos.size() << "\n";
            for(size_t i = 0; i < puntos.size(); i++){
                cout << sangria << "   - ";
                mostrarpunto(puntos[i]);
                cout << "\n";
            }
        }
        else{
            cout << " interno numero_puntos=" << numero_puntos << "\n";
            for(int i = 0; i < 8; i++){
                hijos[i]->imprimir(indent + 1);
            }
        }
    }


};


bool cargararchivo(const string& nombre , int N, Octree*& arbol){
    ifstream archivo(nombre.c_str());
    vector<Point> datos;
    string linea;
    while(getline(archivo,linea)){
        istringstream entrada(linea);
        Point p;
        if(entrada >> p.x >> p.y >> p.z){
            datos.push_back((p));
        }
    }

    double minX = datos[0].x, maxX = datos[0].x;
    double minY = datos[0].y, maxY = datos[0].y;
    double minZ = datos[0].z, maxZ = datos[0].z;

    for (const auto& p : datos) {
        minX = min(minX, p.x); maxX = max(maxX, p.x);
        minY = min(minY, p.y); maxY = max(maxY, p.y);
        minZ = min(minZ, p.z); maxZ = max(maxZ, p.z);
    }
    double lado = max({maxX-minX , maxY - minY, maxZ - minZ }) *1.01;

    Point bottomleft(minX,minY,minZ);



    Octree* nuevo = new Octree(bottomleft , lado, N );
    int total = 0;
    for(size_t i = 0; i < datos.size() ; i++){
        if(nuevo->insert(datos[i])){
            total++;
        }
    }
    delete arbol;
    arbol = nuevo;
    cout<< total << "\n";
    arbol->m_raiz();

    return true;
}


bool leerpunto( Point& p){

    cout<<"coordenadas: ";
    return bool(cin>> p.x >> p.y >> p.z);

}




int main(){
    Octree* arbol = new Octree();
    Point p;
    cargararchivo("aguila.xyz", 1,arbol);
    //arbol->imprimir();

    if(leerpunto(p)){
        double radio , ladodelahoja;
        Point mascercano;
        cout<<"ingresar radio: ";
        cin>> radio;
        cout<<"\n";
        if(arbol->find_closet(p,radio,mascercano,ladodelahoja)){
             mostrarpunto(mascercano);
            cout<<"\n";
            mostrarpunto(mascercano);
            cout<<"distancia: " << sqrt(dist_cuadrada(p,mascercano));
            cout <<"\nlado hoja: " << ladodelahoja << "\n";
        }
        else {
            cout<<"NULL\n";
        }



    }



    return 0;
}
