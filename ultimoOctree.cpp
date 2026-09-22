#include "iostream"
#include "fstream"
#include "sstream"
#include "vector"
#include "string"
#include "cmath"
#include "limits"
#include "iomanip"
#include "algorithm"

using namespace std;

struct Point {
    double x, y, z;

    Point(double a = 0, double b = 0, double c = 0) {
        x = a;
        y = b;
        z = c;
    }
};

void mostrarPunto(const Point& p) {
    cout << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

bool puntoValido(const Point& p) {
    return isfinite(p.x) && isfinite(p.y) && isfinite(p.z);
}

double distanciaCuadrada(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

class Octree {
private:
    Octree* children[8];
    vector<Point> points;
    Point bottomLeft;       // Esquina con los menores valores de x, y, z.
    double h;               // Longitud del lado del cubo.
    int capacidad;          // N: maximo de puntos por hoja antes de dividir.
    int nivel;
    int nPoints;            // Total de puntos de este nodo y sus descendientes.

    bool esHoja() const {
        return children[0] == NULL;
    } 

    bool contiene(const Point& p) const {
        return 
            p.x >= bottomLeft.x && p.x <= bottomLeft.x + h &&
            p.y >= bottomLeft.y && p.y <= bottomLeft.y + h &&
            p.z >= bottomLeft.z && p.z <= bottomLeft.z + h;
    }

    //int elegirHijo(const Point& p) const {
    //    /*calcula a cuál de los 8 octantes(hijos) del cubo actual pertenece el punto p, 
    //        devolviendo un número entre 0 y 7.*/
    //    int hijo = 0;
    //    if (p.x >= bottomLeft.x + h / 2) hijo += 1;
    //    if (p.y >= bottomLeft.y + h / 2) hijo += 2;
    //    if (p.z >= bottomLeft.z + h / 2) hijo += 4;
    //    return hijo;
    //}

 
    /*void dividir() {
        double mitad = h / 2;
        for (int i = 0; i < 8; i++) {
            Point esquina = bottomLeft;
            if (i % 2 == 1) esquina.x += mitad;
            if ((i / 2) % 2 == 1) esquina.y += mitad;
            if (i >= 4) esquina.z += mitad;
            children[i] = new Octree(esquina, mitad, capacidad, nivel + 1);
        }

        for (size_t i = 0; i < points.size(); i++) {
            int hijo = 0;
            if (points[i].x >= bottomLeft.x + mitad) hijo += 1;
            if (points[i].y >= bottomLeft.y + mitad) hijo += 2;
            if (points[i].z >= bottomLeft.z + mitad) hijo += 4;
            children[hijo]->insert(points[i]);
        }
        points.clear();
    }*/
    void dividir() {
        double half = h / 2;
        Point b = bottomLeft;

        Point offsets[8] = {
            {b.x,b.y,b.z},
            {b.x + half, b.y,  b.z},
            {b.x,b.y + half, b.z},
            {b.x + half, b.y + half, b.z},
            {b.x, b.y, b.z + half},
            {b.x + half, b.y, b.z + half},
            {b.x, b.y + half, b.z + half},
            {b.x + half, b.y + half, b.z + half},
        };

        for (int i = 0; i < 8; i++) {
            children[i] = new Octree(offsets[i], half, capacidad, nivel + 1);
        }
        //esto es el centro
        double midX = b.x + half;
        double midY = b.y + half;
        double midZ = b.z + half;
        //Point centro( b.x + half , b.y + half , b.z + half ); es lo mismo xD
        Point centro(midX, midY, midZ);

        for (size_t i = 0; i < points.size(); i++) {
            Point p = points[i];
            bool derecha = p.x >= centro.x;
            bool arriba = p.y >= centro.y;
            bool atras = p.z >= centro.z;

            int indice = 0;
            if (derecha) { 
                indice += 1; 
            }
            if (arriba) { 
                indice += 2; 
            }

            if (atras) { 
                indice += 4; 
            }
            children[indice]->insert(p);
        }

        points.clear();
    }

    double distanciaAlCubo(const Point& p) const {
        // Distancia al cuadrado desde p hasta el punto mas cercano del cubo.
        // Si p esta dentro del cubo, la distancia es cero.
        double dx = 0, dy = 0, dz = 0;
        if (p.x < bottomLeft.x) dx = bottomLeft.x - p.x;
        else if (p.x > bottomLeft.x + h) dx = p.x - (bottomLeft.x + h);
        if (p.y < bottomLeft.y) dy = bottomLeft.y - p.y;
        else if (p.y > bottomLeft.y + h) dy = p.y - (bottomLeft.y + h);
        if (p.z < bottomLeft.z) dz = bottomLeft.z - p.z;
        else if (p.z > bottomLeft.z + h) dz = p.z - (bottomLeft.z + h);
        return dx * dx + dy * dy + dz * dz;
    }

    void buscarCercano(const Point& p, double& mejorDistancia,
        Point& resultado, bool& encontrado, double& ladoHoja) const {
        // No visitamos cubos vacios o demasiado alejados.
        if (nPoints == 0 || distanciaAlCubo(p) > mejorDistancia) return;

        if (esHoja()) {
            for (size_t i = 0; i < points.size(); i++) {
                double d = distanciaCuadrada(p, points[i]);
                if (d <= mejorDistancia && (!encontrado || d < mejorDistancia)) {
                    mejorDistancia = d;
                    resultado = points[i];
                    ladoHoja = h;
                    encontrado = true;
                }
            }
        }
        else {
            // Hay que revisar todos los hijos que puedan contener la respuesta,
            // incluso si el punto consultado pertenece a otro octante.
            for (int i = 0; i < 8; i++) {
                children[i]->buscarCercano(p, mejorDistancia, resultado,
                    encontrado, ladoHoja);
            }
        }
    }

    void escribirCubo(ofstream& archivo, int& vertices) const {
        // OBJ: primero los 8 vertices y luego las 6 caras del cubo.
        for (int i = 0; i < 8; i++) {
            double x = bottomLeft.x + (i % 2) * h;
            double y = bottomLeft.y + ((i / 2) % 2) * h;
            double z = bottomLeft.z + (i / 4) * h;
            archivo << "v " << x << " " << y << " " << z << "\n";
        }
        int caras[6][4] = {
            {1, 3, 4, 2}, {5, 6, 8, 7}, {1, 2, 6, 5},
            {3, 7, 8, 4}, {1, 5, 7, 3}, {2, 4, 8, 6}
        };
        for (int i = 0; i < 6; i++) {
            archivo << "f";
            for (int j = 0; j < 4; j++) archivo << " " << vertices + caras[i][j];
            archivo << "\n";
        }
        vertices += 8;
    }

    void escribirHojas(ofstream& archivo, int& vertices) const {
        // Las hojas vacias se omiten para que se vea la forma de los datos.
        if (nPoints == 0) return;
        if (esHoja()) escribirCubo(archivo, vertices);
        else {
            for (int i = 0; i < 8; i++) children[i]->escribirHojas(archivo, vertices);
        }
    }

public:
    Octree(Point esquina = Point(0, 0, 0), double lado = 128,
        int N = 100, int profundidad = 0) {
        bottomLeft = esquina;
        h = lado;
        capacidad = N > 0 ? N : 1;
        nivel = profundidad;
        nPoints = 0;
        for (int i = 0; i < 8; i++) children[i] = NULL;
    }

    ~Octree() {
        for (int i = 0; i < 8; i++) delete children[i];
    }

    bool exist(const Point& p) const {
        if (!contiene(p)) return false;

        if (!esHoja()) {
            double mitad = h / 2;
            int hijo = 0;
            if (p.x >= bottomLeft.x + mitad) hijo += 1;
            if (p.y >= bottomLeft.y + mitad) hijo += 2;
            if (p.z >= bottomLeft.z + mitad) hijo += 4;
            return children[hijo]->exist(p);
        }

        for (size_t i = 0; i < points.size(); i++) {
            if (points[i].x == p.x && points[i].y == p.y && points[i].z == p.z)
                return true;
        }
        return false;
    }

    bool insert(const Point& p) {
        if (!contiene(p)) return false;

        if (esHoja()) {
            if (exist(p)) return false;
            if (points.size() < static_cast<size_t>(capacidad)) {
                points.push_back(p);
                nPoints++;
                return true;
            }
            dividir();
        }

        double mitad = h / 2;
        int hijo = 0;
        if (p.x >= bottomLeft.x + mitad) hijo += 1;
        if (p.y >= bottomLeft.y + mitad) hijo += 2;
        if (p.z >= bottomLeft.z + mitad) hijo += 4;

        bool insertado = children[hijo]->insert(p);
        if (insertado) nPoints++;
        return insertado;
    }

    // Devuelve false si no hay respuesta. Las referencias permiten devolver
    // el punto y el lado de su hoja sin inventar una coordenada para NULL.
    // El radio incluye su borde. Si p ya existe, puede ser su propio vecino.
    // En un empate se conserva el primer punto encontrado.
    bool find_closest(const Point& p, double radius, Point& resultado,
        double& ladoHoja) const {
        ladoHoja = 0;
        double mejorDistancia = radius * radius;
        bool encontrado = false;
        buscarCercano(p, mejorDistancia, resultado, encontrado, ladoHoja);
        return encontrado;
    }

    void imprimir() const {
        for (int i = 0; i < nivel; i++) cout << "  ";
        cout << (esHoja() ? "Hoja " : "Nodo ");
        mostrarPunto(bottomLeft);
        cout << " h=" << h << " puntos=" << nPoints << "\n";
        if (esHoja()) {
            for (size_t i = 0; i < points.size(); i++) {
                for (int j = 0; j <= nivel; j++) cout << "  ";
                mostrarPunto(points[i]);
                cout << "\n";
            }
        }
        else {
            for (int i = 0; i < 8; i++) children[i]->imprimir();
        }
    }

    void mostrarRaiz() const {
        cout << "bottomLeft = ";
        mostrarPunto(bottomLeft);
        cout << "\nh = " << h << "\nN = " << capacidad;
        cout << "\nPuntos guardados = " << nPoints << "\n";
    }

    bool exportarOBJ(const string& nombre) const {
        ofstream archivo(nombre.c_str());
        if (!archivo) return false;
        archivo << setprecision(17);
        archivo << "# Cubos de las hojas ocupadas del Octree\no hojas\n";
        int vertices = 0;
        escribirHojas(archivo, vertices);
        archivo.close();
        return !archivo.fail();
    }
};

bool leerPunto(Point& p) {
    cout << "Ingrese x y z: ";
    return bool(cin >> p.x >> p.y >> p.z) ;
}

// Lee las primeras tres columnas; permite columnas adicionales (color, normales).
// Las lineas vacias y los comentarios que empiezan con # se omiten.
bool cargarXYZ(const string& nombre, int N, Octree*& arbol) {
    ifstream archivo(nombre.c_str());
   /* if (!archivo) {
        cout << "No se pudo abrir el archivo.\n";
        return false;
    }*/

    vector<Point> datos;
    string linea;
    while (getline(archivo, linea)) {
        istringstream entrada(linea);
       
        Point p;
        if (entrada >> p.x >> p.y >> p.z ) datos.push_back(p);
    }
    double minX = datos[0].x, maxX = datos[0].x;
    double minY = datos[0].y, maxY = datos[0].y;
    double minZ = datos[0].z, maxZ = datos[0].z;

    for (const auto& p : datos) {
        minX = min(minX, p.x); maxX = max(maxX, p.x);
        minY = min(minY, p.y); maxY = max(maxY, p.y);
        minZ = min(minZ, p.z); maxZ = max(maxZ, p.z);
    }

    //Point minimo = datos[0], maximo = datos[0];
    //for (const auto& p : datos) {
    //    minimo.x = min(minimo.x, p.x); 
    //    maximo.x = max(maximo.x, p.x);
    //    minimo.y = min(minimo.y, p.y); 
    //    maximo.y = max(maximo.y, p.y);
    //    minimo.z = min(minimo.z, p.z); 
    //    maximo.z = max(maximo.z, p.z);
    //}

    /*for (size_t i = 1; i < datos.size(); i++) {
        if (datos[i].x < minimo.x) minimo.x = datos[i].x;
        if (datos[i].y < minimo.y) minimo.y = datos[i].y;
        if (datos[i].z < minimo.z) minimo.z = datos[i].z;
        if (datos[i].x > maximo.x) maximo.x = datos[i].x;
        if (datos[i].y > maximo.y) maximo.y = datos[i].y;
        if (datos[i].z > maximo.z) maximo.z = datos[i].z;
    }*/

    double lado = max({ maxX - minX,maxY - minY,maxZ - minZ }) * 1.01;
    // Un lado potencia de 2 facilita las divisiones sucesivas.
    //double lado = 1;
    //while (minimo.x + lado < maximo.x || minimo.y + lado < maximo.y ||
    //    minimo.z + lado < maximo.z) {
    //    lado *= 1.01;
    //}
    Point minimo(minX, minY, minZ); 

    Octree* nuevo = new Octree(minimo, lado, N);
    int guardados = 0;
    for (size_t i = 0; i < datos.size(); i++) {
        if (nuevo->insert(datos[i])) guardados++;
    }
    delete arbol; //liberamos al puntero arbol
    arbol = nuevo;//lo asignamos al arbol "nuevo"
    cout << "Puntos insertados: " << guardados;
    cout << "\nRepetidos omitidos: " << datos.size() - guardados;
    //cout << "\nLineas invalidas omitidas: " << invalidas << "\n";
    arbol->mostrarRaiz();
    return true;
}

int main() {
    Octree* arbol = new Octree();
   /* cout << setprecision(12);
    cout << "OCTREE\nCubo inicial: (0,0,0), lado 128, N=100.\n";
    cout << "Cargar un XYZ o crear un cubo reemplaza el arbol actual.\n";*/
    //7 y 8 no
    int opcion = -1;
    while (opcion != 0) {
        cout << "\n1. Cargar archivo XYZ\n2. Insertar punto\n3. Verificar punto";
        cout << "\n4. Buscar punto mas cercano\n5. Imprimir arbol";
        cout << "\n6. Mostrar datos de la raiz\n7. Exportar hojas a OBJ";
        cout << "\n8. Crear un cubo vacio\n0. Salir\nOpcion: ";
        if (!(cin >> opcion)) {
            if (cin.eof()) break;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ingrese una opcion numerica.\n";
            continue;
        }

        Point p;
        if (opcion == 1) {
            string nombre;
            int N;
            cout << "Ruta del archivo XYZ (sin comillas): ";
            getline(cin >> ws, nombre);
            cout << "Capacidad N por hoja (por ejemplo, 100): ";
            if (cin >> N && N > 0) cargarXYZ(nombre, N, arbol);
            else cout << "N debe ser un entero positivo.\n";
        }
        else if (opcion == 2) {
            if (!leerPunto(p)) cout << "Coordenadas invalidas.\n";
            else if (arbol->insert(p)) cout << "Punto insertado.\n";
            else cout << "El punto ya existe o esta fuera del cubo.\n";
        }
        else if (opcion == 3) {
            if (!leerPunto(p)) cout << "Coordenadas invalidas.\n";
            else cout << (arbol->exist(p) ? "El punto existe.\n" : "El punto no existe.\n");
        }
        else if (opcion == 4) {
            if (leerPunto(p)) {
                double radio, ladoHoja;
                Point cercano;
                cout << "Radio (por ejemplo, 25): ";
                if (!(cin >> radio) || !isfinite(radio) || radio < 0) {
                    cout << "Radio invalido.\n";
                }
                else if (arbol->find_closest(p, radio, cercano, ladoHoja)) {
                    cout << "X = ";
                    mostrarPunto(cercano);
                    cout << "\nDistancia = " << sqrt(distanciaCuadrada(p, cercano));
                    cout << "\nh de la hoja de X = " << ladoHoja << "\n";
                }
                else cout << "X = NULL\n";
            }
            else cout << "Coordenadas invalidas.\n";
        }
        else if (opcion == 5) {
            arbol->imprimir();
        }
        else if (opcion == 6) {
            arbol->mostrarRaiz();
        }
      /*  else if (opcion == 7) {
            string nombre;
            cout << "Nombre del archivo de salida (ejemplo: octree.obj): ";
            getline(cin >> ws, nombre);
            if (arbol->exportarOBJ(nombre)) cout << "Archivo OBJ creado para MeshLab.\n";
            else cout << "No se pudo escribir el archivo OBJ.\n";
        }*/
       /* else if (opcion == 8) {
            double lado;
            int N;
            cout << "Esquina inferior del cubo. ";
            if (leerPunto(p)) {
                cout << "Lado del cubo y capacidad N: ";
                if (cin >> lado >> N && isfinite(lado) && lado > 0 && N > 0 &&
                    puntoValido(Point(p.x + lado, p.y + lado, p.z + lado)) &&
                    p.x + lado > p.x && p.y + lado > p.y && p.z + lado > p.z) {
                    delete arbol;
                    arbol = new Octree(p, lado, N);
                    cout << "Cubo vacio creado.\n";
                }
                else cout << "Lado o capacidad invalidos.\n";
            }
            else cout << "Coordenadas invalidas.\n";
        }*/
        else if (opcion != 0) {
            cout << "Opcion invalida.\n";
        }

        if (cin.eof()) break;
        if (cin.fail()) cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    delete arbol;
    return 0;
}
