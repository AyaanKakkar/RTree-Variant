#include <saveGraph.h>
#include <vector>
#include <fstream> 


using namespace std;

void saveData(string fileName, vector<int> x, vector<int> y) {
    ofstream f(fileName);

    for (int i = 0; i < x.size(); i++) {
        f << x[i] << "," << y[i] << "\n";
    }

    f.close();
}

void saveData(string fileName, vector<int> x, vector<long double> y) {
    ofstream f(fileName);

    for (int i = 0; i < x.size(); i++) {
        f << x[i] << "," << y[i] << "\n";
    }

    f.close();
}

void saveData(string fileName, vector<long double> x, vector<int> y) {
    ofstream f(fileName);

    for (int i = 0; i < x.size(); i++) {
        f << x[i] << "," << y[i] << "\n";
    }

    f.close();
}

void saveData(string fileName, vector<long double> x, vector<long double> y) {
    ofstream f(fileName);

    for (int i = 0; i < x.size(); i++) {
        f << x[i] << "," << y[i] << "\n";
    }

    f.close();
}