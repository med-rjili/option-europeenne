#ifndef MATRICE_HPP_INCLUDED
#define MATRICE_HPP_INCLUDED

#include "vecteur.hpp"
#include "matrice.hpp"
#include <iostream>

using namespace std;

class matrice
{
  private:
    vector<pair <int,int>> indexes; 
    Vecteur<double> valeurs;//coeficients de la matrice
    

  public:   
    int dim; //dimension de la matrice
    matrice(vector<pair <int,int>> indexes,Vecteur<double> valeurs,int dim);
    matrice(int dim=0) : dim(dim) {}
    int val(int i,int j)const;
    Vecteur<double> matrix_vector(const Vecteur<double>& v);
    void print(std::ostream &out=std::cout) const;
    friend std::ostream &operator<<(std::ostream &out, const matrice &mat);
    matrice& operator=(const matrice& other);
    matrice& operator+=(const matrice& mat);
    matrice& operator*=(const double alpha);
    matrice& operator-=(const matrice& mat);
    matrice& operator/=(const double alpha);
    double operator()(int row, int col) const;
    void set(int row,int col,double val);
    pair<matrice, matrice> factorisationLU() const;
    Vecteur<double> resoudre(const matrice& L, const matrice& U, const Vecteur<double>& b);


};
matrice operator+(const matrice& mat1, const matrice& mat2);
matrice operator-(const matrice& mat1, const matrice& mat2);
matrice operator*(double alpha, const matrice& mat);
matrice operator/(const matrice& mat,double alpha);

class matrice_symetrique:public matrice
{
  private:

  public:

};
class matrice_Nonsymetrique:public matrice
{
  private:

  public:
};

#endif