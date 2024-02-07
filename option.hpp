#ifndef OPTION_HPP_INCLUDED
#define OPTION_HPP_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <tuple>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <math.h>
using namespace std;
//---------------------------------------------------------------------------
//     classe vecteur
//---------------------------------------------------------------------------
class vecteur
{
private :
  int dim_;          // dimension du vecteur

public:
  double * val_;     // tableaux de valeurs
  void init(int d);        //allocation
  vecteur(int d=0, double v0=0); // dim et val constante
  vecteur(const vecteur & v);    // constructeur par copie
  int dim() const {return dim_;}
  vecteur & operator =(const vecteur & v);
  vecteur & operator =(const double &x);//
  double  operator [](const int & i) const;//
  double & operator [] (const int & i);//
  double  operator () (const int & i) const;//
  double & operator () (const int & i);//
  friend ostream & operator<<(ostream & out, const vecteur & v);//
  friend istream & operator>>(istream & in, const vecteur & v);//
  vecteur operator() (const int &i, const int &j) const;//
  vecteur& operator +=(const vecteur & v);//
  vecteur& operator -=(const vecteur & v);//
  vecteur& operator +=(const double& x);//
  vecteur& operator -=(const double& x);//
  vecteur& operator *=(const double& x);//
  vecteur& operator /=(const double& x);//
};
vecteur operator +(const vecteur& u);//
vecteur operator -(const vecteur& u);//
vecteur operator +(const vecteur& u,const vecteur& v);//
vecteur operator +(const vecteur& u,const double& x);//
vecteur operator +(const double& x,const vecteur& u);//
vecteur operator -(const vecteur& u,const vecteur& v);//
vecteur operator -(const vecteur& u,const double& x);//
vecteur operator -(const double& x,const vecteur& u);//
vecteur operator *(const vecteur& u,const double& x);//
vecteur operator *(const double& x,const vecteur& u);//
vecteur operator /(const vecteur& u,const double& x);//
bool operator ==(const vecteur& u, const vecteur& v);//
bool operator !=(const vecteur& u, const vecteur& v);//
vecteur operator |(const vecteur& u,const vecteur& v);//
vecteur operator ,(const vecteur& u,const vecteur& v);//
istream & operator>>(istream & in, const vecteur & v);//
ostream & operator <<(ostream & out, const vecteur & v);//

#endif // OPTION_HPP_INCLUDED
