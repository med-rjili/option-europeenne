#include "pricing.hpp"
#include "elementsFinies.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
// Structured P1 mesh gives half-bandwidth n+1. Factor once, reuse each time step.
class BandMatrix {
    int size_, band_;
    std::vector<double> data_;
public:
    BandMatrix(int size, int band): size_(size), band_(band), data_(size*(2*band+1), 0) {}
    double& at(int i, int j) { return data_[i*(2*band_+1)+j-i+band_]; }
    double at(int i, int j) const { return data_[i*(2*band_+1)+j-i+band_]; }
    void identityRow(int i) {
        for (int j=std::max(0,i-band_); j<=std::min(size_-1,i+band_); ++j) at(i,j) = (i==j);
    }
    Vecteur<double> multiply(const Vecteur<double>& x) const {
        Vecteur<double> y(size_,0);
        for (int i=0;i<size_;++i)
            for (int j=std::max(0,i-band_);j<=std::min(size_-1,i+band_);++j) y[i]+=at(i,j)*x[j];
        return y;
    }
    void factor() {
        for (int k=0;k<size_;++k) {
            if (!std::isfinite(at(k,k)) || std::abs(at(k,k))<1e-15)
                throw std::runtime_error("FEM band LU: invalid pivot; refine/change grid");
            for (int i=k+1;i<=std::min(size_-1,k+band_);++i) {
                at(i,k)/=at(k,k);
                for (int j=k+1;j<=std::min(size_-1,k+band_);++j) at(i,j)-=at(i,k)*at(k,j);
            }
        }
    }
    Vecteur<double> solve(Vecteur<double> b) const {
        for (int i=0;i<size_;++i)
            for (int j=std::max(0,i-band_);j<i;++j) b[i]-=at(i,j)*b[j];
        for (int i=size_-1;i>=0;--i) {
            for (int j=i+1;j<=std::min(size_-1,i+band_);++j) b[i]-=at(i,j)*b[j];
            b[i]/=at(i,i);
            if (!std::isfinite(b[i])) throw std::runtime_error("Non-finite FEM solution");
        }
        return b;
    }
};
double boundary(const Point& s, const PricingParameters& p, double tau) {
    const auto c = mat_cov();
    if (s.x==0) return blackScholesCall(s.y,p.strike,p.rate,std::sqrt(c(1,1)),tau);
    if (s.y==0) return blackScholesCall(s.x,p.strike,p.rate,std::sqrt(c(0,0)),tau);
    // Upper edges: deep-ITM asymptote (domain truncation approximation).
    return std::max(s.x+s.y-p.strike*std::exp(-p.rate*tau),0.0);
}
}

FemResult priceFiniteElements(const PricingParameters& p, double domain, int n, int steps,
                              std::vector<Vecteur<double>>* history) {
    p.validate();
    if (!std::isfinite(domain) || domain<=0 || domain<p.s1 || domain<p.s2 || n<2 || n>500 || steps<1)
        throw std::invalid_argument("Invalid FEM domain/grid/time steps (2 <= subdivisions <= 500)");
    Maillage mesh(0,domain,0,domain,n,n);
    const int count = static_cast<int>(mesh.points.size());
    Vecteur<double> values(count,0);
    for (int i=0;i<count;++i) values[i]=std::max(mesh.points[i].x+mesh.points[i].y-p.strike,0.0);
    if (history) { history->clear(); history->push_back(values); }
    if (p.maturity==0) return {mesh,values};
    const double dt=p.maturity/steps;
    BandMatrix left(count,n+1), right(count,n+1);
    for (const auto& tri : mesh.triangles) {
        const auto s1=mesh.points[tri.s1], s2=mesh.points[tri.s2], s3=mesh.points[tri.s3];
        const auto m=matM_el(s1,s2,s3), k=matK_el(s1,s2,s3), b=matB_el(s1,s2,s3,p.rate);
        const int ids[3]={tri.s1,tri.s2,tri.s3};
        for (int i=0;i<3;++i) for (int j=0;j<3;++j) {
            const double d=p.rate*m(i,j)+k(i,j)+b(i,j);
            left.at(ids[i],ids[j])+=m(i,j)+0.5*dt*d;
            right.at(ids[i],ids[j])+=m(i,j)-0.5*dt*d;
        }
    }
    std::vector<int> edges;
    for (int i=0;i<=n;++i) for (int j=0;j<=n;++j)
        if (i==0 || j==0 || i==n || j==n) {
            edges.push_back(i*(n+1)+j);
            left.identityRow(edges.back());
        }
    left.factor();
    for (int step=1;step<=steps;++step) {
        auto rhs=right.multiply(values);
        for (int id:edges) rhs[id]=boundary(mesh.points[id],p,step*dt);
        values=left.solve(rhs);
        if (history) history->push_back(values);
    }
    return {mesh,values};
}

double FemResult::priceAt(double x, double y) const {
    if (!std::isfinite(x) || !std::isfinite(y)) throw std::invalid_argument("Non-finite spot");
    for (const auto& t:mesh.triangles) {
        const Point a=mesh.points[t.s1], b=mesh.points[t.s2], c=mesh.points[t.s3];
        const double det=(b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
        const double u=((x-a.x)*(c.y-a.y)-(y-a.y)*(c.x-a.x))/det;
        const double v=((b.x-a.x)*(y-a.y)-(b.y-a.y)*(x-a.x))/det;
        if (u>=-1e-12 && v>=-1e-12 && u+v<=1+1e-12)
            return (1-u-v)*values[t.s1]+u*values[t.s2]+v*values[t.s3];
    }
    throw std::out_of_range("Spot outside FEM mesh");
}
