#include "matrice.hpp"
#include "vecteur.hpp"

using namespace std;


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

void matrice::print(std::ostream &out) const {
  map <pair<int,int>,double> mp;
  bool exist[dim][dim] = {{false}};
  for(int i=0;i<indexes.size();i++){
    exist[indexes[i].first][indexes[i].second]=true;
    mp[indexes[i]]=valeurs[i];
  }
  for(int i=0;i<dim;i++){
    out<<"(";
    for(int j=0;j<dim-1;j++){
      if (exist[i][j]) out<< mp[pair<int,int> (i,j)]<<" ";
      else out<<0<<" ";
    }
    if (exist[i][dim-1]) out<< mp[pair<int,int> (i,dim-1)]<<")\n";
    else out<<"0)\n";
  }
}
std::ostream &operator<<(std::ostream &out, const matrice &mat) {
    mat.print(out);
    return out;
}
matrice& matrice::operator=(const matrice& other)
{
    if (this != &other) {
        indexes = other.indexes;
        valeurs = other.valeurs;
        dim = other.dim;
    }
    return *this;
}

matrice& matrice::operator+=(const matrice& mat){
        map<pair<int,int>,double> mp;
        for (int i=0;i<indexes.size();i++){
        map<pair<int,int>,double>::iterator it = mp.find(indexes[i]);
        if(it != mp.end()){
            mp[indexes[i]]+=valeurs[i];
            if (mp[indexes[i]]==0) mp.erase(indexes[i]);
          }
          else{
            mp[indexes[i]]=valeurs[i];
          }
        }
        for (int i=0;i<mat.indexes.size();i++){
          map <pair<int,int>,double>::iterator it = mp.find(mat.indexes[i]);
          if(it != mp.end()){
            mp[mat.indexes[i]]+=mat.valeurs[i];
            if (mp[indexes[i]]==0) mp.erase(mat.indexes[i]);
          }
          else{
            mp[mat.indexes[i]]=mat.valeurs[i];
          }

        }
        Vecteur <double> result_values(mp.size());
        vector<pair<int,int>> result_indexes;
        int idx=0;
        for(const auto& it :mp){
          result_values[idx++]=it.second;
          result_indexes.push_back(it.first);
        }
        valeurs=result_values;
        indexes=result_indexes;
        return *this;
}

matrice& matrice::operator*=(const double alpha){
  for(int i=0;i<valeurs.get_dim();i++){
    valeurs[i]*=alpha;
  }
  return *this;
}

matrice& matrice::operator-=(const matrice& mat){
  map<pair<int,int>,double> mp;
  for (int i=0;i<indexes.size();i++){
    map<pair<int,int>,double>::iterator it = mp.find(indexes[i]);
    if(it != mp.end()){
      mp[indexes[i]]+=valeurs[i];
      if (mp[indexes[i]]==0) mp.erase(indexes[i]);
    }
    else{
      mp[indexes[i]]=valeurs[i];
    }
  }
  for (int i=0;i<mat.indexes.size();i++){
    map <pair<int,int>,double>::iterator it = mp.find(mat.indexes[i]);
    if(it != mp.end()){
      mp[mat.indexes[i]]-=mat.valeurs[i];
      if (mp[indexes[i]]==0) mp.erase(mat.indexes[i]);
    }
    else{
      mp[mat.indexes[i]]=-mat.valeurs[i];
    }

  }
  Vecteur <double> result_values(mp.size());
  vector<pair<int,int>> result_indexes;
  int idx=0;
  for(const auto& it :mp){
    result_values[idx++]=it.second;
    result_indexes.push_back(it.first);
  }
  valeurs=result_values;
  indexes=result_indexes;
  return *this;
}
matrice& matrice::operator/=(const double alpha){
  if(alpha==0){
    cout<<"division by zero error !!"<<endl;
    exit(-1);
  }
  else *this*=1.0/alpha;
  return *this;
}

matrice operator+(const matrice& mat1, const matrice& mat2)
{
  matrice res=mat1;
  return res+=mat2;
}
matrice operator-(const matrice& mat1, const matrice& mat2){
  matrice res=mat1;
  return res-=mat2;
}
matrice operator*( double alpha, const matrice& mat){
  matrice res=mat;
  return res*=alpha;
}
matrice operator/(const matrice& mat,double alpha){
  matrice res=mat;
  return res/=alpha;
}


double matrice::operator()(int row, int col) const{
  for (int i=0;i<indexes.size();i++){
    if(indexes[i].first==row and indexes[i].second==col){
      return valeurs[i];
    }
  }
  return 0.0;
}

void matrice::set(int row,int col,double val){
  for(int i=0;i<indexes.size();i++){
    if(indexes[i].first==row and indexes[i].second==col){
      if(val==0.0){
        valeurs.erase(valeurs.begin()+i);
        indexes.erase(indexes.begin()+i);
      }
      else{
        valeurs[i]=val;
      }
      return;
    }
  } 
  if (val!=0.0){
    indexes.push_back(pair<int,int> (row,col));
    valeurs.push_back(val);
  }
    
}



// Fonction de factorisation LU
pair<matrice, matrice> matrice::factorisationLU() const {
    matrice L(dim), U(dim);
    for (int i = 0; i < dim; i++) {
        L.set(i, i, 1); // Initialize diagonal of L with 1
        for (int j = i; j < dim; j++) {
            double sum = 0.0;
            for (int k = 0; k < i; k++) {
                sum += L(i, k) * U(k, j);
            }
            U.set(i, j, (*this)(i, j) - sum); // Update U using L and previously computed U values
        }
        for (int j = i + 1; j < dim; j++) {
            double sum = 0.0;
            for (int k = 0; k < i; k++) {
                sum += L(j, k) * U(k, i);
            }
            L.set(j, i, ((*this)(j, i) - sum) / U(i, i)); // Update L using previously computed L values and U
        }
    }
    return { L, U };
}

//resolution du systeme LUX=b 

Vecteur<double> matrice::resoudre(const matrice& L, const matrice& U, const Vecteur<double>& b) {
    int n = L.dim;
    Vecteur<double> y(n); // Pour stocker les solutions de Ly=b
    Vecteur<double> x(n); // Pour stocker les solutions de Ux=y

    // Résolution de Ly=b (système triangulaire inférieur)
    for (int i = 0; i < n; ++i) {
        double sum = 0.0;
        for (int j = 0; j < i; ++j) {
            sum += L.val(i, j) * y[j];
        }
        y[i] = (b[i] - sum) / L.val(i, i);
    }

    // Résolution de Ux=y (système triangulaire supérieur)
    for (int i = n - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int j = i + 1; j < n; ++j) {
            sum += U.val(i, j) * x[j];
        }
        x[i] = (y[i] - sum) / U.val(i, i);
    }

    return x;
}