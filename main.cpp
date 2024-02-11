#include <iostream>
#include "maillage.hpp"
#include <bits/stdc++.h>
#include <vector>
using namespace std;

int main()
{
    vector<pair<int, int>> indexes = { {0, 0}, {0, 1}, {1, 0}};
    Vecteur<double> valeurs(3,4);
    Vecteur<double> b(2, 2);

    // Assuming `matrice` constructor now takes `std::vector` instead of `Vecteur`
    matrice A(indexes, valeurs, 2);
    matrice L, U;
    cout<<"A = \n"<<A<<endl;
    tie(L, U) = A.factorisationLU();
    Vecteur<double> X = A.resoudre(L, U, b);

    cout << "matrice L:\n" << L << endl;
    cout << "matrice U:\n" << U << endl;
    cout << "La solution du système LUX=b est:\n" << X << endl;

    return 0;
}
