#include "option.hpp"
//--------------------------------------------------------------------------
//                       class matrice
//--------------------------------------------------------------------------
matrice::matrice(vector<pair <int,int>> indexes,Vecteur<double> valeurs,int dim):indexes(indexes),valeurs(valeurs),dim(dim){}
Vecteur<double>& matrice::matix_vector(const Vecteur<double>& v)
{
    Vecteur<double> result(indexes.size(),0);
    for (int i=0;i<dim;i++)
    {
        for(int j=0;j<indexes.size();j++)
        {
            if(indexes[j].first==i){
            result[i]+=valeurs[j]*v[indexes[j].second];
            }
        }
    }
    return result;
}

int matrice::val(int i,int j)const
    { 
      for (int k=0;k<indexes.size();k++)
      {
        if (indexes[k].first==i and indexes[k].second==j)return valeurs[k];
      }
      return 0;
    }
