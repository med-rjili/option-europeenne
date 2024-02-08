#include <iostream>
#include "option.hpp"
#include <bits/stdc++.h>
using namespace std;

int main()
{

    vector <pair<int,int>> indexes1;
    indexes1.push_back(pair<int,int> (0,0));
    indexes1.push_back(pair<int,int> (1,1));
    indexes1.push_back(pair<int,int> (2,2));
    indexes1.push_back(pair<int,int> (0,1));
    indexes1.push_back(pair<int,int> (0,2));

    Vecteur <double> valeurs1(5,2);
    matrice mat1(indexes1,valeurs1,3);
    cout<<mat1;

    vector<pair<int,int>> indexes2;
    indexes2.push_back(pair<int,int> (0,0));
    indexes2.push_back(pair<int,int> (1,1));
    indexes2.push_back(pair<int,int> (2,2));

    Vecteur <double> valeurs2(3,-2);
    matrice mat2(indexes2,valeurs2,3);
    cout<<"\n\n";
    cout<<mat2;

/*
    Vecteur <double> v(3,1);

    cout<<mat.matrix_vector(v)<<endl;
    cout<<"\n\n";
    matrice res;
    res = mat1.add_matrix(mat2);
    cout<<res;
*/

cout<<endl;
cout<<mat1+mat2<<endl;
cout<<mat1-mat2<<endl;
cout<<2*mat1-5*mat2<<endl;

return 0;
}
