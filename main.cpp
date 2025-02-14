#include <iostream>
#include "WeightsFileManager.h"

using namespace std;

int main()
{
    int nodes;
    int version;

    cout << "Insert nodes: ";
    cin >> nodes;

    cout << "File version: ";
    cin >> version;

    WeightsFileManager wm(50);
    wm.createWeights(nodes, version);
    cout << "* File created" << endl;
    return 0;
}