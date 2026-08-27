#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

double calcular_distancia_euclidiana(const vector<double>& a, const vector<double>& b) {
    double suma = 0;
    for (int i = 0; i < a.size(); i++) {
        suma += pow(a[i] - b[i], 2);
    }
    return sqrt(suma);
}

int main() {
    vector<vector<double>> vector_10;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    int dimension;
    cout << "Ingrese la dimension: "; cin >> dimension;

    //Generar los 100 puntos en una dimension k
    for (int i = 0; i < 100; i++) {
        vector<double> punto;
        for (int j = 0; j < dimension; j++) {
            punto.push_back(dis(gen));
        }
        vector_10.push_back(punto);
    }

	string nombre_archivo = "distancias_" + to_string(dimension) + ".csv";
	ofstream file(nombre_archivo);
    file << "Distancia\n";

	if (!file) {
		cout << "No se pudo abrir el archivo" << endl;
		return 1;
	}

    cout << "Puntos generados en " << dimension << " dimensiones" << endl;
    for (int i = 0; i < min(100, (int)vector_10.size()); i++) {
        for (double coord : vector_10[i]) {
            cout << coord << " ";
        }
        cout << endl;
    }

    for (int i = 0; i < 100; i++) {
        for (int j = i + 1; j < 100; j++) {
            double distancias_euclidianas = calcular_distancia_euclidiana(vector_10[i], vector_10[j]);
            file << distancias_euclidianas << "\n";
            cout << "Distancia entre punto " << i << " y punto " << j << ": " << distancias_euclidianas << endl;
        }
    }
	file.close();
    cout << "\nLas distancias se han guardado en '" << nombre_archivo << "'.\n";
    return 0;
}