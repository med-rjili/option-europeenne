#include <iostream>
#include "maillage.hpp"
#include <bits/stdc++.h>
#include <vector>
#include "elementsFinies.hpp" 
#include "vecteur.hpp"
using namespace std;

int main()
{
    vector<pair<int, int>> indexes = { {0, 0},{1, 0},{2,0},{3,0},{4,0},{2,1},{2,2},{2,3},{2,4}};
    Vecteur<double> valeurs(9,1);
    Vecteur<double> b(5,2);
    matrice mat(indexes,valeurs,5,5);
    cout<<mat<<endl;

    //for(int i=0;i<5;i++)cout<<mat.getRow(i)<<endl;
    Point S1(0,0),S2(0,1),S3(1,0);
    matrice K=matK_el(S1,S2,S3);
    matrice B=matB_el(S1,S2,S3);
    cout<<K<<endl;
    cout<<B<<endl;
    // Assuming `matrice` constructor now takes `std::vector` instead of `Vecteur`
   /* vector <Point> v1;
    Vecteur<double> v2;
    auto quad2_res=quad_2();
    v1=quad2_res.first;
    v2=quad2_res.second;
    for(auto i=v1.begin();i!=v1.end();i++) cout<<*i<<endl;
    cout<<v2<<endl;*/




    /*
    
    matrice L, U;
    cout<<"A = \n"<<A<<endl;
    tie(L, U) = A.factorisationLU();
    Vecteur<double> X = A.resoudre(L, U, b);

    cout << "matrice L:\n" << L << endl;
    cout << "matrice U:\n" << U << endl;
    cout << "La solution du système LUX=b est:\n" << X << endl;

    */

    return 0;
}
