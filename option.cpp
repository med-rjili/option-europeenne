#include "option.hpp"
//---------------------------------------------------------------------------
//     classe vecteur
//---------------------------------------------------------------------------
void vecteur::init(int d) //initialisation avec allocation dynamique
{
  if(d<0) stop("init() : dimension <0");
  dim_=d; val_=0;
  if(d>0) val_ = new double[d];
}

vecteur::vecteur(int d, double v0):dim_(d),val_(0) // dim et val constante
{
  init(d);
  for (int i=0;i<dim_;i++) val_[i]=v0;
}

vecteur::vecteur(const vecteur & v): dim_(v.dim_), val_(0) // constructeur par copie
{
  init(v.dim_);
  for (int i=0;i<dim_;i++) val_[i]=v[i];
}



vecteur & vecteur::operator =(const vecteur & v)
{
    if (this==&v) {return *this;}

    for (int i=0;i<dim_;i++)
    {
        val_[i]=v.val_[i];
    }
    return *this;
}
vecteur & vecteur:: operator =(const double &x)
{
    for(int i=0;i<dim_;i++) val_[i]=x;
    return(*this);
}
double vecteur:: operator [] (const int &i) const
{
    return (val_[i]);
}
double & vecteur:: operator [] (const int &i)
{
    return (val_[i]);
}
double vecteur:: operator () (const int &i) const
{
    return (val_[i-1]);
}
double & vecteur:: operator () (const int &i)
{
    return (val_[i-1]);
}
ostream & operator <<(ostream & out, const vecteur & v)
{
    out<<"(";
    for (int i=0;i<v.dim()-1;i++) {out<<v.val_[i]<<",";}
    out <<v.val_[v.dim_-1]<<")";
    return out;
}
istream & operator>>(istream & in, const vecteur & v)
{
    for (int i=0;i<v.dim_;i++)
        in>> v.val_[i];
    return in;
}
vecteur  vecteur:: operator() (const int &i, const int &j) const
{

    vecteur v(j-i+1);
    for (int k=0;k<j-i+1;k++)
    {
        v.val_[k]=val_[k+i-1];
    }
    return(v);
}
vecteur& vecteur::operator +=(const vecteur& v)
{
    test_dim(dim_,v.dim(),"erreur");
    for (int i=0;i<dim_;i++)
        val_[i]=val_[i]+v[i];
    return *this;
}
vecteur&vecteur::operator -=(const vecteur & v)
{
     test_dim(dim_,v.dim(),"erreur");
    for (int i=0;i<dim_;i++)
        val_[i]-=v[i];
    return *this;
}
vecteur&vecteur::operator +=(const double& x)
{
    for (int i=0;i<dim_;i++)
        val_[i]+=x;
    return *this;
}
vecteur& vecteur::operator -=(const double& x)
{
    for (int i=0;i<dim_;i++)
        val_[i]-=x;
    return *this;
}
vecteur& vecteur::operator *=(const double& x)
{
    for (int i=0;i<dim_;i++)
        val_[i]*=x;
    return *this;
}
vecteur& vecteur::operator /=(const double& x)
{
    if(x==0) {cout<<"division par 0!"<<endl; exit(-1);}
    for (int i=0;i<dim_;i++)
        val_[i]/=x;
    return *this;
}
vecteur operator +(const vecteur& u)
{
    return u;
}
vecteur operator -(const vecteur& u)
{
    vecteur s(u);
    for (int i=0;i<u.dim();i++) s[i]*=-1;
    return s;
}
vecteur operator +(const vecteur& u,const vecteur& v)
{
    vecteur s(u);
    s+=v;
    return(s);
}
vecteur operator +(const vecteur& u,const double& x)
{
    vecteur s(u);
    s+=x;
    return(s);
}
vecteur operator +(const double& x,const vecteur& u)
{
    return(u+x);
}
vecteur operator -(const vecteur& u,const vecteur& v)
{
    return(u+(-v));
}
vecteur operator -(const vecteur& u,const double& x)
{
    vecteur s(u);
    s-=x;
    return(s);
}
vecteur operator -(const double& x,const vecteur& u)
{
    return(x+(-u));
}
vecteur operator *(const vecteur& u,const double& x)
{
    vecteur s(u);
    s*=x;
    return(s);
}
vecteur operator *(const double& x,const vecteur& u)
{
    return(u*x);
}
vecteur operator /(const vecteur& u,const double& x)
{
    vecteur s(u);
    s/=x;
    return(s);
}
bool operator ==(const vecteur& u, const vecteur& v)
{
    if(u.dim()!=v.dim()) return false;
    for (int i=0;i<u.dim();i++)
        if (u[i]!=v[i]) return false;
    return true;
}
bool operator !=(const vecteur& u, const vecteur& v)
{
    return(!(u==v));
}
vecteur operator |(const vecteur& u,const vecteur& v)
{
    test_dim(u.dim(),v.dim(),"erreur");
    double s=0;
    for (int i=0;i<u.dim();i++)
        s+=u[i]*v[i];
    return s;
}
vecteur operator ,(const vecteur& u,const vecteur& v)
{
    vecteur s(u.dim()+v.dim());
    for (int i=0;i<u.dim();i++)
        s[i]=u[i];
    for (int j=0;j<v.dim();j++)
        s[j+u.dim()]=v[j];
    return s;
}
operande operator~(const vecteur& u)
{
    return(operande(&u));
}
vecteur operator*(const vecteur& u,const operande& ov)
{
    const vecteur& v=*ov.vp;
    test_dim(u.dim(),v.dim(),"erreur");
    vecteur s(u.dim());
    for(int i=0;i<u.dim();i++)
        s[i]=u[i]*v[i];
    return s;
}
vecteur operator/(const vecteur& u,const operande& ov)
{
    const vecteur& v=*ov.vp;
    test_dim(u.dim(),v.dim(),"erreur");
    vecteur s(u.dim());
    for(int i=0;i<u.dim();i++)
    {
        if(v(i)==0) {cout<<"division par 0!"<<endl; exit(-1);}
        s[i]=u[i]/v[i];
    }
    return s;
}
