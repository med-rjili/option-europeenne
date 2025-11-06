#include "monteCarlo.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>

MonteCarloPricer::MonteCarloPricer(const OptionParams& params, int num_simulations, unsigned int seed)
    : params(params), num_simulations(num_simulations), rng(seed) {}

std::pair<double, double> MonteCarloPricer::generateCorrelatedNormals(double rho) {
    std::normal_distribution<double> normal(0.0, 1.0);
    
    double Z1 = normal(rng);
    double Z2 = normal(rng);
    
    // Use Cholesky decomposition to correlate the normals
    double W1 = Z1;
    double W2 = rho * Z1 + std::sqrt(1 - rho * rho) * Z2;
    
    return std::make_pair(W1, W2);
}

std::pair<double, double> MonteCarloPricer::simulatePrices(double Z1, double Z2) {
    // Simulate prices using geometric Brownian motion
    // S_T = S_0 * exp((r - 0.5*sigma^2)*T + sigma*sqrt(T)*Z)
    
    double sqrt_T = std::sqrt(params.T);
    
    double S1_T = params.S1_0 * std::exp((params.r - 0.5 * params.sigma1 * params.sigma1) * params.T 
                                         + params.sigma1 * sqrt_T * Z1);
    
    double S2_T = params.S2_0 * std::exp((params.r - 0.5 * params.sigma2 * params.sigma2) * params.T 
                                         + params.sigma2 * sqrt_T * Z2);
    
    return std::make_pair(S1_T, S2_T);
}

double MonteCarloPricer::payoff(double S1, double S2) {
    // Payoff for a European call on the sum of two assets
    return std::max(S1 + S2 - params.K, 0.0);
}

double MonteCarloPricer::computePrice() {
    std::vector<double> payoffs(num_simulations);
    
    for (int i = 0; i < num_simulations; ++i) {
        auto [Z1, Z2] = generateCorrelatedNormals(params.rho);
        auto [S1_T, S2_T] = simulatePrices(Z1, Z2);
        payoffs[i] = payoff(S1_T, S2_T);
    }
    
    // Calculate mean
    double sum = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    double mean = sum / num_simulations;
    
    // Discount to present value
    double price = mean * std::exp(-params.r * params.T);
    
    return price;
}

double MonteCarloPricer::computePriceAntithetic() {
    std::vector<double> payoffs(num_simulations);
    
    for (int i = 0; i < num_simulations / 2; ++i) {
        // Generate correlated normals
        auto [Z1, Z2] = generateCorrelatedNormals(params.rho);
        
        // Path 1: positive normals
        auto [S1_T_pos, S2_T_pos] = simulatePrices(Z1, Z2);
        double payoff_pos = payoff(S1_T_pos, S2_T_pos);
        
        // Path 2: antithetic (negative) normals
        auto [S1_T_neg, S2_T_neg] = simulatePrices(-Z1, -Z2);
        double payoff_neg = payoff(S1_T_neg, S2_T_neg);
        
        // Average the two payoffs
        payoffs[2*i] = payoff_pos;
        payoffs[2*i + 1] = payoff_neg;
    }
    
    // Calculate mean
    double sum = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    double mean = sum / num_simulations;
    
    // Discount to present value
    double price = mean * std::exp(-params.r * params.T);
    
    return price;
}

MonteCarloResult MonteCarloPricer::priceOption() {
    std::vector<double> payoffs(num_simulations);
    
    for (int i = 0; i < num_simulations; ++i) {
        auto [Z1, Z2] = generateCorrelatedNormals(params.rho);
        auto [S1_T, S2_T] = simulatePrices(Z1, Z2);
        payoffs[i] = payoff(S1_T, S2_T);
    }
    
    // Calculate mean and standard error
    double sum = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    double mean = sum / num_simulations;
    
    double sq_sum = std::inner_product(payoffs.begin(), payoffs.end(), payoffs.begin(), 0.0);
    double variance = (sq_sum / num_simulations) - (mean * mean);
    double std_error = std::sqrt(variance / num_simulations);
    
    // Discount to present value
    double price = mean * std::exp(-params.r * params.T);
    double discounted_std_error = std_error * std::exp(-params.r * params.T);
    
    MonteCarloResult result;
    result.price = price;
    result.std_error = discounted_std_error;
    result.greeks = calculateGreeks(price);
    
    return result;
}

MonteCarloResult MonteCarloPricer::priceOptionAntithetic() {
    std::vector<double> payoffs(num_simulations);
    
    for (int i = 0; i < num_simulations / 2; ++i) {
        // Generate correlated normals
        auto [Z1, Z2] = generateCorrelatedNormals(params.rho);
        
        // Path 1: positive normals
        auto [S1_T_pos, S2_T_pos] = simulatePrices(Z1, Z2);
        double payoff_pos = payoff(S1_T_pos, S2_T_pos);
        
        // Path 2: antithetic (negative) normals
        auto [S1_T_neg, S2_T_neg] = simulatePrices(-Z1, -Z2);
        double payoff_neg = payoff(S1_T_neg, S2_T_neg);
        
        // Average the two payoffs
        payoffs[2*i] = payoff_pos;
        payoffs[2*i + 1] = payoff_neg;
    }
    
    // Calculate mean and standard error
    double sum = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    double mean = sum / num_simulations;
    
    double sq_sum = std::inner_product(payoffs.begin(), payoffs.end(), payoffs.begin(), 0.0);
    double variance = (sq_sum / num_simulations) - (mean * mean);
    double std_error = std::sqrt(variance / num_simulations);
    
    // Discount to present value
    double price = mean * std::exp(-params.r * params.T);
    double discounted_std_error = std_error * std::exp(-params.r * params.T);
    
    MonteCarloResult result;
    result.price = price;
    result.std_error = discounted_std_error;
    result.greeks = calculateGreeks(price);
    
    return result;
}

Greeks MonteCarloPricer::calculateGreeks(double base_price) {
    Greeks greeks;
    
    // Small perturbations for finite differences
    // Using adaptive step sizes based on parameter values
    double dS = 0.01 * std::max(params.S1_0, params.S2_0);  // 1% of max spot price
    double dSigma = 0.01;            // 1% absolute change in volatility
    double dr = 0.0001;              // 1 basis point
    double dt = std::min(1.0 / 365.0, params.T / 10.0);  // 1 day or 10% of time, whichever is smaller
    
    // Use different random seeds for each Greek calculation to avoid bias
    unsigned int seed_base = rng();  // Get a new seed from the main RNG
    
    // Delta 1: sensitivity to S1
    OptionParams params_S1_up = params;
    params_S1_up.S1_0 += dS;
    MonteCarloPricer pricer_S1_up(params_S1_up, num_simulations, seed_base + 1);
    double price_S1_up = pricer_S1_up.computePriceAntithetic();
    
    OptionParams params_S1_down = params;
    params_S1_down.S1_0 -= dS;
    MonteCarloPricer pricer_S1_down(params_S1_down, num_simulations, seed_base + 2);
    double price_S1_down = pricer_S1_down.computePriceAntithetic();
    
    greeks.delta1 = (price_S1_up - price_S1_down) / (2 * dS);
    
    // Delta 2: sensitivity to S2
    OptionParams params_S2_up = params;
    params_S2_up.S2_0 += dS;
    MonteCarloPricer pricer_S2_up(params_S2_up, num_simulations, seed_base + 3);
    double price_S2_up = pricer_S2_up.computePriceAntithetic();
    
    OptionParams params_S2_down = params;
    params_S2_down.S2_0 -= dS;
    MonteCarloPricer pricer_S2_down(params_S2_down, num_simulations, seed_base + 4);
    double price_S2_down = pricer_S2_down.computePriceAntithetic();
    
    greeks.delta2 = (price_S2_up - price_S2_down) / (2 * dS);
    
    // Gamma 1: second derivative with respect to S1
    greeks.gamma1 = (price_S1_up - 2 * base_price + price_S1_down) / (dS * dS);
    
    // Gamma 2: second derivative with respect to S2
    greeks.gamma2 = (price_S2_up - 2 * base_price + price_S2_down) / (dS * dS);
    
    // Vega 1: sensitivity to sigma1
    OptionParams params_sigma1_up = params;
    params_sigma1_up.sigma1 += dSigma;
    MonteCarloPricer pricer_sigma1_up(params_sigma1_up, num_simulations, seed_base + 5);
    double price_sigma1_up = pricer_sigma1_up.computePriceAntithetic();
    
    greeks.vega1 = (price_sigma1_up - base_price) / dSigma;
    
    // Vega 2: sensitivity to sigma2
    OptionParams params_sigma2_up = params;
    params_sigma2_up.sigma2 += dSigma;
    MonteCarloPricer pricer_sigma2_up(params_sigma2_up, num_simulations, seed_base + 6);
    double price_sigma2_up = pricer_sigma2_up.computePriceAntithetic();
    
    greeks.vega2 = (price_sigma2_up - base_price) / dSigma;
    
    // Rho (Greek): sensitivity to correlation
    if (params.rho < 0.99) {
        OptionParams params_rho_up = params;
        params_rho_up.rho += 0.01;
        MonteCarloPricer pricer_rho_up(params_rho_up, num_simulations, seed_base + 7);
        double price_rho_up = pricer_rho_up.computePriceAntithetic();
        
        greeks.rho_greek = (price_rho_up - base_price) / 0.01;
    } else {
        greeks.rho_greek = 0.0;
    }
    
    // Theta: time decay
    if (params.T > dt) {
        OptionParams params_t_down = params;
        params_t_down.T -= dt;
        MonteCarloPricer pricer_t_down(params_t_down, num_simulations, seed_base + 8);
        double price_t_down = pricer_t_down.computePriceAntithetic();
        
        greeks.theta = (price_t_down - base_price) / dt;
    } else {
        greeks.theta = 0.0;
    }
    
    // Rho (rate): sensitivity to interest rate
    OptionParams params_r_up = params;
    params_r_up.r += dr;
    MonteCarloPricer pricer_r_up(params_r_up, num_simulations, seed_base + 9);
    double price_r_up = pricer_r_up.computePriceAntithetic();
    
    greeks.rho_rate = (price_r_up - base_price) / dr;
    
    return greeks;
}

std::vector<double> MonteCarloPricer::priceEvolution(int num_time_steps) {
    std::vector<double> prices(num_time_steps);
    double dt = params.T / num_time_steps;
    
    for (int step = 0; step < num_time_steps; ++step) {
        OptionParams params_t = params;
        params_t.T = dt * (step + 1);
        
        MonteCarloPricer pricer(params_t, num_simulations, 42);
        prices[step] = pricer.computePriceAntithetic();
    }
    
    return prices;
}

std::vector<std::pair<int, double>> MonteCarloPricer::convergenceAnalysis(const std::vector<int>& simulation_counts) {
    std::vector<std::pair<int, double>> results;
    
    for (int count : simulation_counts) {
        MonteCarloPricer pricer(params, count, 42);
        double price = pricer.computePriceAntithetic();
        results.push_back(std::make_pair(count, price));
    }
    
    return results;
}

std::vector<double> priceGrid(const OptionParams& params, 
                              const std::vector<double>& S1_values,
                              const std::vector<double>& S2_values,
                              int num_simulations) {
    std::vector<double> prices;
    
    for (double S1 : S1_values) {
        for (double S2 : S2_values) {
            OptionParams params_temp = params;
            params_temp.S1_0 = S1;
            params_temp.S2_0 = S2;
            
            MonteCarloPricer pricer(params_temp, num_simulations, 42);
            double price = pricer.priceOption().price;
            prices.push_back(price);
        }
    }
    
    return prices;
}

double analyticalPrice(const OptionParams& params) {
    // For a sum of two log-normal random variables, there's no closed-form solution
    // We can approximate using moment matching or other techniques
    // For now, return 0 to indicate no analytical solution is available
    // This would need to be implemented with numerical integration or approximation methods
    return 0.0;
}
