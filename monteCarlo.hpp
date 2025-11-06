#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <vector>
#include <random>
#include <cmath>
#include "vecteur.hpp"

// Structure to hold option parameters
struct OptionParams {
    double S1_0;      // Initial price of asset 1
    double S2_0;      // Initial price of asset 2
    double K;         // Strike price
    double T;         // Time to maturity (in years)
    double r;         // Risk-free rate
    double sigma1;    // Volatility of asset 1
    double sigma2;    // Volatility of asset 2
    double rho;       // Correlation between assets
};

// Structure to hold Greeks
struct Greeks {
    double delta1;    // Delta with respect to asset 1
    double delta2;    // Delta with respect to asset 2
    double gamma1;    // Gamma with respect to asset 1
    double gamma2;    // Gamma with respect to asset 2
    double vega1;     // Vega with respect to volatility of asset 1
    double vega2;     // Vega with respect to volatility of asset 2
    double rho_greek; // Sensitivity to correlation
    double theta;     // Time decay
    double rho_rate;  // Sensitivity to interest rate
};

// Structure to hold Monte Carlo results
struct MonteCarloResult {
    double price;
    double std_error;
    Greeks greeks;
};

// Monte Carlo pricing class
class MonteCarloPricer {
private:
    OptionParams params;
    int num_simulations;
    std::mt19937 rng;
    
    // Generate correlated normal random variables using Cholesky decomposition
    std::pair<double, double> generateCorrelatedNormals(double rho);
    
    // Simulate asset prices at maturity
    std::pair<double, double> simulatePrices(double Z1, double Z2);
    
    // Calculate payoff for the option
    double payoff(double S1, double S2);
    
    // Internal pricing method without Greeks calculation
    double computePrice();
    double computePriceAntithetic();
    
public:
    MonteCarloPricer(const OptionParams& params, int num_simulations, unsigned int seed = 42);
    
    // Price the option using standard Monte Carlo
    MonteCarloResult priceOption();
    
    // Price the option using antithetic variates for variance reduction
    MonteCarloResult priceOptionAntithetic();
    
    // Calculate Greeks using finite differences
    Greeks calculateGreeks(double price);
    
    // Price evolution over time
    std::vector<double> priceEvolution(int num_time_steps);
    
    // Convergence analysis
    std::vector<std::pair<int, double>> convergenceAnalysis(const std::vector<int>& simulation_counts);
};

// Utility functions for visualization and analysis
std::vector<double> priceGrid(const OptionParams& params, 
                              const std::vector<double>& S1_values,
                              const std::vector<double>& S2_values,
                              int num_simulations);

// Black-Scholes analytical solution for comparison (if available)
double analyticalPrice(const OptionParams& params);

#endif // MONTE_CARLO_HPP
