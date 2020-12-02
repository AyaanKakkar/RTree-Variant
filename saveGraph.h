#ifndef _SAVEGRAPH
#define _SAVEGRAPH

#include <vector>
#include <string>

using namespace std;

void saveData(string fileName, vector<int> x, vector<int> y);
void saveData(string fileName, vector<int> x, vector<long double> y);
void saveData(string fileName, vector<long double> x, vector<int> y);
void saveData(string fileName, vector<long double> x, vector<long double> y);

#endif