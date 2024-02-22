#ifndef MATRICE_HPP_INCLUDED
#define MATRICE_HPP_INCLUDED

#include "vecteur.hpp"
#include <iostream>
#include <vector>
#include <map>

class matrice
{
private:
    std::map<std::pair<int, int>, double> data;
    int rows; // number of rows
    int cols; // number of columns

public:
    matrice();
    matrice(int rows,int cols);
    matrice(int rows, int cols, double val);
    double val(int i, int j) const;
    Vecteur<double> matrix_vector(const Vecteur<double> &v) const ;
    void print(std::ostream &out = std::cout) const;
    friend std::ostream &operator<<(std::ostream &out, const matrice &mat);
    matrice &operator=(const matrice &other);
    matrice &operator+=(const matrice &mat);
    matrice &operator*=(const double alpha);
    matrice &operator-=(const matrice &mat);
    matrice &operator/=(const double alpha);
    double operator()(int row, int col) const;
    void set(int row, int col, double val);
    std::pair<matrice, matrice> factorisationLU() const;
    Vecteur<double> getRow(int row);
    void setRow(int row,const Vecteur <double> &v);
    int getNbRows();
    int getNbCols();
};

matrice operator+(const matrice &mat1, const matrice &mat2);
matrice operator-(const matrice &mat1, const matrice &mat2);
matrice operator*(double alpha, const matrice &mat);
Vecteur<double> operator*(const matrice &mat, const Vecteur<double> &vect);
matrice operator/(const matrice &mat, double alpha);
Vecteur<double> resoudre(const matrice &A,const Vecteur<double> &b);
#endif
