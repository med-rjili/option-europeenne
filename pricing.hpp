#ifndef PRICING_HPP
#define PRICING_HPP
#include "maillage.hpp"
#include <cstdint>
#include <cstddef>

// Annual units; no dividends. Covariance is shared with FEM via mat_cov().
struct PricingParameters {
    double s1 = 0.5, s2 = 0.5, strike = 1.0, rate = 0.05, maturity = 2.0;
    void validate() const;
};
struct MonteCarloResult {
    double price, standard_error, ci_low, ci_high;
    std::size_t pairs; // Independent observations; two antithetic paths per pair.
};
MonteCarloResult priceMonteCarlo(const PricingParameters&, std::size_t pairs = 500000,
                                std::uint64_t seed = 42);
struct FemResult {
    Maillage mesh;
    Vecteur<double> values;
    double priceAt(double s1, double s2) const; // P1 interpolation, same triangles.
};
FemResult priceFiniteElements(const PricingParameters&, double domain = 2.0,
                             int subdivisions = 40, int time_steps = 244,
                             std::vector<Vecteur<double>>* history = nullptr);
double blackScholesCall(double spot, double strike, double rate, double sigma, double maturity);
#endif
