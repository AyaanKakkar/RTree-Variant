#include "saveGraph.h"
#include <vector>
#include <fstream>

using namespace std;

void saveGraph(string fileName, vector<int> x, vector<int> y) {
    ofstream f(fileName);
    
    for (int i = 0; i < x.size(); i++) {
        f << x[i] << "," << y[i] << "\n";
    }

    f.close();
}

void saveGraph(string fileName, vector<double> x, vector<int> y) {
    ofstream f(fileName);
    
    for (int i = 0; i < x.size(); i++) {
        f << x[i] << "," << y[i] << "\n";
    }

    f.close();
}