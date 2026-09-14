#include "Octree.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

int main(int argc, char* argv[]) {
    try {
        string entrada = argc > 1 ? argv[1] : "aguila.xyz";
        int capacidad = argc > 2 ? stoi(argv[2]) : 4;
        string nombre = argc > 3 ? argv[3] : "aguila_4";
        ifstream archivo(entrada);
        if (!archivo) throw runtime_error("No se pudo abrir " + entrada);

        vector<Point> datos;
        string linea;
        int numeroLinea = 0;

        while (getline(archivo, linea)) {
            numeroLinea++;
            if (linea.find_first_not_of(" \t\r") == string::npos) continue;
            istringstream lectura(linea);
            Point p;
            if (!(lectura >> p.x >> p.y >> p.z) ||
                !isfinite(p.x) || !isfinite(p.y) || !isfinite(p.z)) {
                throw runtime_error("Punto invalido en la linea " + to_string(numeroLinea));
            }
            datos.push_back(p);
        }
        if (datos.empty()) throw runtime_error("El archivo no contiene puntos.");

        Point minimo = datos[0], maximo = datos[0];
        for (const Point& p : datos) {
            minimo.x = min(minimo.x, p.x);
            minimo.y = min(minimo.y, p.y);
            minimo.z = min(minimo.z, p.z);
            maximo.x = max(maximo.x, p.x);
            maximo.y = max(maximo.y, p.y);
            maximo.z = max(maximo.z, p.z);
        }

        double lado = max({maximo.x - minimo.x, maximo.y - minimo.y,
                           maximo.z - minimo.z});
        lado = max(1.0, lado) + 0.000001;
        Octree arbol(minimo, lado, capacidad);
        for (const Point& p : datos) arbol.insert(p);

        cout << setprecision(12) << "Puntos leidos: " << datos.size() << '\n';
        cout << "Capacidad por hoja: " << capacidad << '\n';
        cout << "bottomLeft: (" << minimo.x << ", " << minimo.y << ", " << minimo.z << ")\n";
        cout << "Lado de la raiz: " << lado << '\n';

        arbol.exportarOBJ(nombre + "_hojas.obj");
        arbol.exportarOBJ(nombre + "_raiz.obj", true);
        ofstream salida(nombre + "_arbol.txt");
        if (!salida) throw runtime_error("No se pudo guardar el arbol.");
        arbol.print(salida);
        salida.close();
        if (!salida) throw runtime_error("No se pudo completar el archivo del arbol.");
        cout << "Archivos generados con prefijo: " << nombre << '\n';

        if (argc > 4 && string(argv[4]) == "--consultar") {
            int opcion;
            while (true) {
                cout << "\n1. Buscar punto\n2. Punto mas cercano\n3. Imprimir arbol\n0. Salir\n";
                if (!(cin >> opcion) || opcion == 0) break;
                if (opcion == 3) {
                    arbol.print();
                } else if (opcion == 1 || opcion == 2) {
                    Point p;
                    cout << "Ingrese x y z: ";
                    if (!(cin >> p.x >> p.y >> p.z)) break;
                    if (opcion == 1) {
                        cout << (arbol.exist(p) ? "El punto existe.\n" : "El punto no existe.\n");
                    } else {
                        int radio;
                        cout << "Radio entero: ";
                        if (!(cin >> radio)) break;
                        try {
                            Point cercano = arbol.find_closest(p, radio);
                            cout << "Punto: (" << cercano.x << ", " << cercano.y << ", " << cercano.z << ")\n";
                        } catch (const exception& e) {
                            cout << e.what() << '\n';
                        }
                    }
                } else {
                    cout << "Opcion invalida.\n";
                }
            }
        }
    } catch (const exception& e) {
        cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
