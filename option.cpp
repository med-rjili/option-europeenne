#include "option.hpp"
//--------------------------------------------------------------------------
//                       class matrice
//--------------------------------------------------------------------------
matrice::matrice(vector<pair <int,int>> indexes,Vecteur<double> valeurs,int dim):indexes(indexes),valeurs(valeurs),dim(dim){}
Vecteur<double> matrice::matrix_vector(const Vecteur<double>& v)
{
    Vecteur<double> result(dim,0);
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

void matrice::print() const {
  map <pair<int,int>,double> mp;
  bool exist[dim][dim] = {{false}};
  for(int i=0;i<indexes.size();i++){
    exist[indexes[i].first][indexes[i].second]=true;
    mp[indexes[i]]=valeurs[i];
  }
  for(int i=0;i<dim;i++){
    cout<<"(";
    for(int j=0;j<dim-1;j++){
      if (exist[i][j]) cout<< mp[pair<int,int> (i,j)]<<" ";
      else cout<<0<<" ";
    }
    if (exist[i][dim-1]) cout<< mp[pair<int,int> (i,dim-1)]<<")\n";
    else cout<<"0)\n";
  }

}
