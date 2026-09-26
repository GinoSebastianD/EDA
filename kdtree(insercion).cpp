#include <bits/stdc++.h>
using namespace std;
struct Nodo{
    double x, y;
    Nodo* izq;
    Nodo* der;
};

Nodo* creando(double x, double y){
    Nodo* n = new Nodo;
    n->x = x;
    n->y = y;
    n->izq = nullptr;
    n->der = nullptr;
    return n;
}

Nodo* insertar(Nodo* raiz , double x , double y , int profundidad){
    if(raiz == nullptr){
        return creando(x,y);
    }

    int eje = profundidad %2; //alternamos los ejes X y Y
    if (eje == 0){
        if (x < raiz->x){
            raiz->izq = insertar(raiz->izq , x, y , profundidad +1);
        }
        else{
            raiz->der = insertar(raiz->der,x,y,profundidad+1);
        }
    }
    else {
        if(y < raiz->y){
            raiz->izq = insertar(raiz->izq , x, y , profundidad +1);
        }
         else{
            raiz->der = insertar(raiz->der,x,y,profundidad+1);
        }
    }
    return raiz;

}

void imprimir(Nodo* raiz){
    if(raiz == nullptr){
        return;
    }
    imprimir(raiz->izq);
    cout<< raiz->x << " " <<raiz->y <<"\n";
    imprimir(raiz->der);


}



int main(){

    Nodo * raiz = nullptr;
    raiz = insertar(raiz, 3, 6, 0);
    raiz = insertar(raiz, 17, 15, 0);
    raiz = insertar(raiz, 13, 15, 0);
    raiz = insertar(raiz, 6, 12, 0);
    raiz = insertar(raiz, 9, 1, 0);
    raiz = insertar(raiz, 2, 7, 0);
    raiz = insertar(raiz, 10, 19, 0);

    imprimir(raiz);


}
