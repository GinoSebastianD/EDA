#include "vector"
#include "iostream"
#include "random"
#include "cmath"
#include "fstream"
#include "string"
#include "algorithm"

using namespace std;

double distancia(const vector<double>& a, const vector<double>& b) {
	double suma = 0;
	for ( int i = 0 ; i < a.size() ; ++i )
	{
		suma = suma + pow(a[i]-b[i],2);
	}
	return sqrt(suma);
}

void imprimir(const vector<vector<double>>& vect ) {
	for (int i = 0 ; i < vect.size() ; ++i)
	{
		for (int j = 0 ; j < vect[i].size(); ++j)
		{
			cout << vect[i][j] << " ";
		}
		cout << endl;
	}
}


int main() {
	vector<vector<double>> vect;

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> dis(0.0, 1.0);

	int dim;
	cout << "dimension: ";  cin >> dim;
	
	for (int i = 0 ; i < 100 ; ++i)
	{
		vector<double> punto;
		for (int j = 0 ; j < dim; j++ )
		{
			punto.push_back(dis(gen));
		}
		vect.push_back(punto);
	}

	string nombre = "nombre" + to_string(dim) + ".csv";
	ofstream archivo(nombre);
	archivo << "Distancia\n";

	imprimir(vect);

	for (int  i = 0; i < 100; i++)
	{
		for (int j = i +1 ; j < 100 ; j++)
		{
			double eucli = distancia(vect[i], vect[j]);
			archivo << eucli << "\n";
		}
	}
	archivo.close();

}






