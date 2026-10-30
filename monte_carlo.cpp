#include "pricing.hpp"
#include "elementsFinies.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

void PricingParameters::validate() const {
    for (double x : {s1, s2, strike, rate, maturity})
        if (!std::isfinite(x)) throw std::invalid_argument("Parameters must be finite");
    if (s1 < 0 || s2 < 0 || strike < 0 || maturity < 0)
        throw std::invalid_argument("Spots, strike and maturity must be nonnegative");
}

double blackScholesCall(double s, double k, double r, double sigma, double t) {
    if (t == 0) return std::max(s-k, 0.0);
    if (s == 0) return 0;
    if (k == 0) return s;
    if (sigma == 0) return std::max(s-k*std::exp(-r*t), 0.0);
    const double v = sigma*std::sqrt(t);
    const double d1 = (std::log(s/k)+(r+sigma*sigma/2)*t)/v;
    auto cdf = [](double x) { return 0.5*std::erfc(-x/std::sqrt(2.0)); };
    return s*cdf(d1)-k*std::exp(-r*t)*cdf(d1-v);
}

MonteCarloResult priceMonteCarlo(const PricingParameters& p, std::size_t pairs, std::uint64_t seed) {
    p.validate();
    if (pairs < 2) throw std::invalid_argument("At least two independent pairs required");
    if (p.maturity == 0) {
        double payoff = std::max(p.s1+p.s2-p.strike, 0.0);
        return {payoff, 0, payoff, payoff, pairs};
    }
    const auto c = mat_cov();
    const double v1 = std::sqrt(c(0,0)), v2 = std::sqrt(c(1,1));
    const double rho = c(0,1)/(v1*v2);
    const double orthogonal = std::sqrt(std::max(0.0, 1-rho*rho));
    const double t = p.maturity, root_t = std::sqrt(t), discount = std::exp(-p.rate*t);
    std::mt19937_64 engine(seed);
    std::normal_distribution<double> normal(0, 1);
    double mean = 0, m2 = 0;
    for (std::size_t i = 0; i < pairs; ++i) {
        const double z1 = normal(engine), independent = normal(engine);
        const double z2 = rho*z1+orthogonal*independent;
        double observation = 0;
        for (double sign : {-1.0, 1.0}) {
            const double s1 = p.s1*std::exp((p.rate-c(0,0)/2)*t+sign*v1*root_t*z1);
            const double s2 = p.s2*std::exp((p.rate-c(1,1)/2)*t+sign*v2*root_t*z2);
            observation += 0.5*discount*std::max(s1+s2-p.strike, 0.0);
        }
        // Welford on pair means: antithetic paths are NOT independent samples.
        const double delta = observation-mean;
        mean += delta/static_cast<double>(i+1);
        m2 += delta*(observation-mean);
    }
    const double se = std::sqrt(m2/static_cast<double>(pairs-1)/static_cast<double>(pairs));
    return {mean, se, mean-1.959963984540054*se, mean+1.959963984540054*se, pairs};
}
