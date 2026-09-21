#include "iostream"
#include "vector"
#include "algorithm"
#include "fstream"
#include "string"
#include "iomanip"
#include <bits/stdc++.h>

using namespace std;

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
        return esquina_inferior_izquierda.x >= p.x && p.x <= esquina_inferior_izquierda.x + h &&
               esquina_inferior_izquierda.y >= p.y && p.y <= esquina_inferior_izquierda.y + h &&
               esquina_inferior_izquierda.z >= p.z && p.z <= esquina_inferior_izquierda.z + h;
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
            if(p.x >= esquina_inferior_izquierda.x ){
                hijo = hijo + 1;
            }
            if(p.y >= esquina_inferior_izquierda.y){
                hijo = hijo + 2;
            }
            if(p.z >= esquina_inferior_izquierda.z){
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


}



int main(){

    return 0;
}
