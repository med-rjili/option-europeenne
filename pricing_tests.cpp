#include "pricing.hpp"
#include "elementsFinies.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

void require(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
void close(double a, double b, double tol, const char* message) { require(std::abs(a-b)<=tol,message); }
template<class F> void rejects(F f) {
    bool rejected=false;
    try { f(); } catch(const std::exception&) { rejected=true; }
    require(rejected,"Invalid input was accepted");
}
int main() try {
    const Point a(0,0), b(1,0), c(0,1);
    const auto mass=matM_el(a,b,c);
    close(mass(0,0),1.0/12,1e-15,"P1 mass diagonal");
    close(mass(0,1),1.0/24,1e-15,"P1 mass off-diagonal");
    // Integral b(x).grad(phi_1)*phi_0 = (0.04-0.012-r)/24.
    const auto convection=matB_el(a,b,c,0.07);
    close(convection(0,1),(0.04-0.012-0.07)/24,1e-15,"Convection quadrature/rate");
    const auto reversed=matB_el(a,c,b,0.07);
    close(reversed(0,2),convection(0,1),1e-15,"Triangle orientation");
    auto times=timeSlots(0,2,0.3);
    close(times.back(),2,0,"Time grid endpoint");
    require(timeSlots(0,2,1).size()==3,"Duplicate endpoint");
    rejects([]{timeSlots(0,2,0);});
    PricingParameters p;
    p.maturity=0;
    p.s1=0.8; p.s2=0.5;
    auto mc=priceMonteCarlo(p,100);
    close(mc.price,0.3,1e-15,"Maturity payoff");
    close(mc.standard_error,0,0,"Maturity variance");
    close(priceFiniteElements(p).priceAt(p.s1,p.s2),0.3,1e-12,"FEM maturity payoff at node");
    p=PricingParameters{}; p.s2=0;
    mc=priceMonteCarlo(p,200000);
    const double exact=blackScholesCall(p.s1,p.strike,p.rate,0.2,p.maturity);
    close(mc.price,exact,5*mc.standard_error,"MC one-asset Black-Scholes limit");
    close(priceFiniteElements(p).priceAt(p.s1,0),exact,1e-12,"FEM axis boundary");
    p=PricingParameters{}; p.strike=0;
    auto fe=priceFiniteElements(p,2,20,100);
    close(fe.priceAt(p.s1,p.s2),p.s1+p.s2,2e-10,"FEM linear martingale solution");
    mc=priceMonteCarlo(p,100000);
    close(mc.price,p.s1+p.s2,5*mc.standard_error,"MC discounted basket expectation");
    p=PricingParameters{};
    const auto first=priceMonteCarlo(p,200000);
    const auto second=priceMonteCarlo(p,200000);
    close(first.price,second.price,0,"Seed reproducibility");
    const double coarse=priceFiniteElements(p,2,20,244).priceAt(p.s1,p.s2);
    const double fine=priceFiniteElements(p,2,80,244).priceAt(p.s1,p.s2);
    require(std::abs(fine-first.price)<std::abs(coarse-first.price),"FEM refinement");
    close(fine,first.price,5*first.standard_error+0.0002,"FEM-MC agreement");
    auto noninteger=priceFiniteElements(p,2.5,20,100);
    close(noninteger.mesh.points.back().x,2.5,0,"Fractional domain preserved");
    rejects([&]{priceMonteCarlo(p,1);});
    rejects([&]{priceFiniteElements(p,0.2);});
    rejects([&]{priceFiniteElements(p,2,0);});
    rejects([&]{priceFiniteElements(p,2,20,0);});
    rejects([&]{noninteger.priceAt(-0.1,0);});
    p.maturity=-1; rejects([&]{priceMonteCarlo(p);});
    matrice zero=0.0*mass; close(zero(0,0),0,0,"Zero scaling");
    std::cout<<"All pricing tests passed\n";
} catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
