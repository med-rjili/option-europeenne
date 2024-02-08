#include <iostream>
#include "option.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{

    vector <pair<int,int>> indexes;
    indexes.push_back(pair<int,int> (0,0));
    indexes.push_back(pair<int,int> (1,1));
    indexes.push_back(pair<int,int> (2,2));
    indexes.push_back(pair<int,int> (0,1));
    indexes.push_back(pair<int,int> (0,2));

    Vecteur <double> valeurs(5,2);
    matrice mat(indexes,valeurs,3);
    mat.print();

    Vecteur <double> v(3,1);

    cout<<mat.matrix_vector(v)<<endl;

    return 0;

}
