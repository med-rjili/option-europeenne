#include "pricing.hpp"
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
    try {
        PricingParameters p;
        int n=40, steps=244;
        std::size_t pairs=500000;
        std::uint64_t seed=42;
        double domain=2;
        std::string output="comparison.csv";
        for (int i=1;i<argc;++i) {
            std::string key=argv[i];
            if (key=="--help") {
                std::cout<<"compare_pricers [--s1 0.5 --s2 0.5 --strike 1 --rate 0.05 --maturity 2]\n"
                           " [--domain 2 --mesh 40 --steps 244 --pairs 500000 --seed 42 --output comparison.csv]\n"
                           "Maturity in YEARS; paths = 2*pairs; covariance from mat_cov().\n";
                return 0;
            }
            if (++i==argc) throw std::invalid_argument("Missing value for "+key);
            const std::string value=argv[i];
            std::size_t used=0;
            if (key=="--output") { output=value; continue; }
            double x=std::stod(value,&used);
            if (used!=value.size() || !std::isfinite(x)) throw std::invalid_argument("Invalid numeric value");
            if (key=="--s1") p.s1=x;
            else if (key=="--s2") p.s2=x;
            else if (key=="--strike") p.strike=x;
            else if (key=="--rate") p.rate=x;
            else if (key=="--maturity") p.maturity=x;
            else if (key=="--domain") domain=x;
            else {
                if (x<0 || x!=std::floor(x) || x>1000000000) throw std::invalid_argument("Invalid integer value");
                if (key=="--mesh") n=static_cast<int>(x);
                else if (key=="--steps") steps=static_cast<int>(x);
                else if (key=="--pairs") pairs=static_cast<std::size_t>(x);
                else if (key=="--seed") seed=static_cast<std::uint64_t>(x);
                else throw std::invalid_argument("Unknown option "+key);
            }
        }
        using Clock=std::chrono::steady_clock;
        const auto start=Clock::now();
        const auto fem=priceFiniteElements(p,domain,n,steps);
        const double fe=fem.priceAt(p.s1,p.s2);
        const auto middle=Clock::now();
        const auto mc=priceMonteCarlo(p,pairs,seed);
        const auto end=Clock::now();
        const double fe_seconds=std::chrono::duration<double>(middle-start).count();
        const double mc_seconds=std::chrono::duration<double>(end-middle).count();
        const double gap=fe-mc.price;
        std::ofstream csv(output);
        if (!csv) throw std::runtime_error("Cannot write "+output);
        const std::string header="s1,s2,strike,rate,maturity_years,domain,mesh,steps,pairs,seed,fem,mc,mc_se,ci_low,ci_high,fem_minus_mc,absolute_gap,relative_gap,inside_mc_ci,fem_seconds,mc_seconds\n";
        csv<<header<<std::setprecision(12)<<p.s1<<','<<p.s2<<','<<p.strike<<','<<p.rate<<','<<p.maturity<<','<<domain<<','<<n<<','<<steps<<','<<pairs<<','<<seed<<','<<fe<<','<<mc.price<<','<<mc.standard_error<<','<<mc.ci_low<<','<<mc.ci_high<<','<<gap<<','<<std::abs(gap)<<',';
        if (mc.price!=0) csv<<std::abs(gap)/std::abs(mc.price);
        csv<<','<<(fe>=mc.ci_low && fe<=mc.ci_high)<<','<<fe_seconds<<','<<mc_seconds<<'\n';
        csv.close();
        if (!csv) throw std::runtime_error("Failed writing "+output);
        std::cout<<std::fixed<<std::setprecision(8)
                 <<"FEM: "<<fe<<" ("<<fe_seconds<<" s)\nMonte Carlo: "<<mc.price<<" ("<<mc_seconds<<" s)\n"
                 <<"MC standard error: "<<mc.standard_error<<"; 95% CI: ["<<mc.ci_low<<", "<<mc.ci_high<<"]\n"
                 <<"FEM - MC: "<<gap<<"; absolute gap: "<<std::abs(gap)<<'\n'
                 <<"CSV: "<<output<<"\nThe MC interval excludes FEM discretization and domain truncation errors.\n";
    } catch (const std::exception& e) { std::cerr<<"Error: "<<e.what()<<'\n'; return 1; }
}
