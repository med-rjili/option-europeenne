#ifndef MAILLAGE_HPP_INCLUDED
#define MAILLAGE_HPP_INCLUDED


#include <iostream>
#include "point.hpp"
#include "triangle.hpp"
#include "matrice.hpp"
#include "vecteur.hpp"
#include <vector>

using namespace std;

class Maillage{
    public:
        vector <Point> points;
        vector <Triangle> triangles;
        int dimH,dimV;
        Maillage(int m, int n); // pas du découpage sur l'abscisse
        void Maillage_carre();
        void print() const;
        Maillage(int a,int b,int c,int d,int m,int n);
        void Maillage_rectangle(int a, int b,int c,int d);
        void Save (const char *fn) const;




};

#endif