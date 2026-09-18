#include "iostream"
#include "vector"
#include "algorithm"
#include "fstream"
#include "string"
#include "iomanip"
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
        return esquina_inferior_izquierda.x >= p.x && p.x <= esquina_inferior_izquierda + h &&
               esquina_inferior_izquierda.y >= p.y && p.y <= esquina_inferior_izquierda + h &&
               esquina_inferior_izquierda.z >= p.z && p.z <= esquina_inferior_izquierda + h;
    }

    bool insert(const Point& p ){
        if(eshoja()){
            
            
            
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
        if(nuevo->   ){
            total++;
        }
    }


}



int main(){

    return 0;
}
