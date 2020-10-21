#include <bits/stdc++.h>
using namespace std;


int main() {

    list<int> ak;

    for (int i = 0; i < 5; i++) {
        ak.push_back(i);
    }

    for (auto it = ak.begin(); it != ak.end(); it++) {
        cout << *it << endl;
    }

    return 0;
}