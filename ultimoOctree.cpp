

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

};



int main(){

    return 0;
}
