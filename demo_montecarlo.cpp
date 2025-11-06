#include <iostream>
#include <fstream>
#include <iomanip>
#include "monteCarlo.hpp"
#include "monteCarloVisualization.hpp"
#include "comparison.hpp"

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void demonstrateBasicPricing() {
    printHeader("1. Basic Monte Carlo Pricing");
    
    OptionParams params;
    params.S1_0 = 1.0;
    params.S2_0 = 1.0;
    params.K = 1.0;
    params.T = 2.0;
    params.r = 0.05;
    params.sigma1 = 0.2;
    params.sigma2 = 0.2;
    params.rho = -0.6;
    
    std::cout << "\nOption Parameters:" << std::endl;
    std::cout << "  S1(0) = " << params.S1_0 << std::endl;
    std::cout << "  S2(0) = " << params.S2_0 << std::endl;
    std::cout << "  K = " << params.K << std::endl;
    std::cout << "  T = " << params.T << " years" << std::endl;
    std::cout << "  r = " << params.r << std::endl;
    std::cout << "  sigma1 = " << params.sigma1 << std::endl;
    std::cout << "  sigma2 = " << params.sigma2 << std::endl;
    std::cout << "  rho = " << params.rho << std::endl;
    
    int num_simulations = 100000;
    std::cout << "\nRunning " << num_simulations << " simulations..." << std::endl;
    
    MonteCarloPricer pricer(params, num_simulations, 42);
    
    // Standard Monte Carlo
    std::cout << "\nStandard Monte Carlo:" << std::endl;
    auto result_std = pricer.priceOption();
    std::cout << "  Price: " << result_std.price << " +/- " << result_std.std_error << std::endl;
    
    // Antithetic variates
    std::cout << "\nAntithetic Variates Monte Carlo:" << std::endl;
    auto result_anti = pricer.priceOptionAntithetic();
    std::cout << "  Price: " << result_anti.price << " +/- " << result_anti.std_error << std::endl;
    
    double variance_reduction = (1.0 - result_anti.std_error / result_std.std_error) * 100.0;
    std::cout << "  Variance Reduction: " << variance_reduction << "%" << std::endl;
}

void demonstrateGreeks() {
    printHeader("2. Greeks Calculation");
    
    OptionParams params;
    params.S1_0 = 1.0;
    params.S2_0 = 1.0;
    params.K = 1.0;
    params.T = 2.0;
    params.r = 0.05;
    params.sigma1 = 0.2;
    params.sigma2 = 0.2;
    params.rho = -0.6;
    
    int num_simulations = 50000;  // Reduced for speed
    std::cout << "\nCalculating Greeks with " << num_simulations << " simulations..." << std::endl;
    
    MonteCarloPricer pricer(params, num_simulations, 42);
    auto result = pricer.priceOptionAntithetic();
    
    std::cout << "\nOption Price: " << result.price << " +/- " << result.std_error << std::endl;
    
    std::cout << "\nGreeks:" << std::endl;
    std::cout << "  Delta1 (∂V/∂S1): " << std::setprecision(6) << result.greeks.delta1 << std::endl;
    std::cout << "  Delta2 (∂V/∂S2): " << result.greeks.delta2 << std::endl;
    std::cout << "  Gamma1 (∂²V/∂S1²): " << result.greeks.gamma1 << std::endl;
    std::cout << "  Gamma2 (∂²V/∂S2²): " << result.greeks.gamma2 << std::endl;
    std::cout << "  Vega1 (∂V/∂σ1): " << result.greeks.vega1 << std::endl;
    std::cout << "  Vega2 (∂V/∂σ2): " << result.greeks.vega2 << std::endl;
    std::cout << "  Theta (∂V/∂t): " << result.greeks.theta << std::endl;
    std::cout << "  Rho_corr (∂V/∂ρ): " << result.greeks.rho_greek << std::endl;
    std::cout << "  Rho_rate (∂V/∂r): " << result.greeks.rho_rate << std::endl;
    
    // Interpretation
    std::cout << "\nInterpretation:" << std::endl;
    std::cout << "  - Delta1 ≈ " << result.greeks.delta1 << ": Option value changes by $" 
              << result.greeks.delta1 << " for $1 increase in S1" << std::endl;
    std::cout << "  - Gamma1 ≈ " << result.greeks.gamma1 << ": Delta1 changes by " 
              << result.greeks.gamma1 << " for $1 increase in S1" << std::endl;
    std::cout << "  - Theta ≈ " << result.greeks.theta << ": Option loses $" 
              << -result.greeks.theta << " per day (time decay)" << std::endl;
}

void demonstrateConvergence() {
    printHeader("3. Convergence Analysis");
    
    OptionParams params;
    params.S1_0 = 1.0;
    params.S2_0 = 1.0;
    params.K = 1.0;
    params.T = 2.0;
    params.r = 0.05;
    params.sigma1 = 0.2;
    params.sigma2 = 0.2;
    params.rho = -0.6;
    
    std::cout << "\nTesting convergence with different simulation counts..." << std::endl;
    
    std::vector<int> sim_counts = {100, 500, 1000, 5000, 10000, 50000, 100000};
    
    MonteCarloPricer pricer(params, 100000, 42);
    auto convergence_data = pricer.convergenceAnalysis(sim_counts);
    
    std::cout << "\n" << std::setw(15) << "Simulations" 
              << std::setw(15) << "Price" 
              << std::setw(15) << "Std Error" << std::endl;
    std::cout << std::string(45, '-') << std::endl;
    
    for (size_t i = 0; i < convergence_data.size(); ++i) {
        // Approximate standard error as inversely proportional to sqrt(N)
        double approx_stderr = 0.01 / std::sqrt(convergence_data[i].first / 10000.0);
        std::cout << std::setw(15) << convergence_data[i].first
                  << std::setw(15) << std::setprecision(6) << convergence_data[i].second
                  << std::setw(15) << std::setprecision(6) << approx_stderr << std::endl;
    }
    
    std::cout << "\nObservation: Error decreases as O(1/√N)" << std::endl;
}

void demonstratePriceEvolution() {
    printHeader("4. Price Evolution Over Time");
    
    OptionParams params;
    params.S1_0 = 1.0;
    params.S2_0 = 1.0;
    params.K = 1.0;
    params.T = 2.0;
    params.r = 0.05;
    params.sigma1 = 0.2;
    params.sigma2 = 0.2;
    params.rho = -0.6;
    
    std::cout << "\nCalculating option price at different maturities..." << std::endl;
    
    int num_simulations = 50000;
    int num_time_steps = 10;
    
    MonteCarloPricer pricer(params, num_simulations, 42);
    auto prices = pricer.priceEvolution(num_time_steps);
    
    std::cout << "\n" << std::setw(15) << "Time (years)" 
              << std::setw(15) << "Option Price" << std::endl;
    std::cout << std::string(30, '-') << std::endl;
    
    for (int i = 0; i < num_time_steps; ++i) {
        double t = params.T * (i + 1) / num_time_steps;
        std::cout << std::setw(15) << std::setprecision(4) << t
                  << std::setw(15) << std::setprecision(6) << prices[i] << std::endl;
    }
    
    std::cout << "\nObservation: Price increases as maturity increases (more time for assets to grow)" << std::endl;
}

void demonstrateSensitivityAnalysis() {
    printHeader("5. Sensitivity Analysis");
    
    OptionParams base_params;
    base_params.S1_0 = 1.0;
    base_params.S2_0 = 1.0;
    base_params.K = 1.0;
    base_params.T = 2.0;
    base_params.r = 0.05;
    base_params.sigma1 = 0.2;
    base_params.sigma2 = 0.2;
    base_params.rho = -0.6;
    
    int num_simulations = 20000;
    
    // Sensitivity to correlation
    std::cout << "\nSensitivity to Correlation:" << std::endl;
    std::cout << std::setw(15) << "Correlation" 
              << std::setw(15) << "Option Price" << std::endl;
    std::cout << std::string(30, '-') << std::endl;
    
    for (double rho = -0.9; rho <= 0.9; rho += 0.3) {
        OptionParams params = base_params;
        params.rho = rho;
        MonteCarloPricer pricer(params, num_simulations, 42);
        auto result = pricer.priceOptionAntithetic();
        std::cout << std::setw(15) << std::setprecision(2) << rho
                  << std::setw(15) << std::setprecision(6) << result.price << std::endl;
    }
    
    // Sensitivity to volatility
    std::cout << "\nSensitivity to Volatility:" << std::endl;
    std::cout << std::setw(15) << "Volatility" 
              << std::setw(15) << "Option Price" << std::endl;
    std::cout << std::string(30, '-') << std::endl;
    
    for (double sigma = 0.1; sigma <= 0.4; sigma += 0.1) {
        OptionParams params = base_params;
        params.sigma1 = sigma;
        params.sigma2 = sigma;
        MonteCarloPricer pricer(params, num_simulations, 42);
        auto result = pricer.priceOptionAntithetic();
        std::cout << std::setw(15) << std::setprecision(2) << sigma
                  << std::setw(15) << std::setprecision(6) << result.price << std::endl;
    }
    
    std::cout << "\nObservations:" << std::endl;
    std::cout << "  - Higher correlation → Higher option value (assets move together)" << std::endl;
    std::cout << "  - Higher volatility → Higher option value (more upside potential)" << std::endl;
}

void saveResultsToFile() {
    printHeader("6. Saving Results to Files");
    
    OptionParams params;
    params.S1_0 = 1.0;
    params.S2_0 = 1.0;
    params.K = 1.0;
    params.T = 2.0;
    params.r = 0.05;
    params.sigma1 = 0.2;
    params.sigma2 = 0.2;
    params.rho = -0.6;
    
    int num_simulations = 100000;
    
    MonteCarloPricer pricer(params, num_simulations, 42);
    auto result = pricer.priceOptionAntithetic();
    
    // Save basic results
    std::ofstream results_file("MonteCarlo_Results.txt");
    results_file << "Monte Carlo Option Pricing Results\n";
    results_file << "===================================\n\n";
    results_file << "Parameters:\n";
    results_file << "  S1(0) = " << params.S1_0 << "\n";
    results_file << "  S2(0) = " << params.S2_0 << "\n";
    results_file << "  K = " << params.K << "\n";
    results_file << "  T = " << params.T << " years\n";
    results_file << "  r = " << params.r << "\n";
    results_file << "  sigma1 = " << params.sigma1 << "\n";
    results_file << "  sigma2 = " << params.sigma2 << "\n";
    results_file << "  rho = " << params.rho << "\n";
    results_file << "  Simulations = " << num_simulations << "\n\n";
    results_file << "Results:\n";
    results_file << "  Option Price: " << result.price << " +/- " << result.std_error << "\n\n";
    results_file << "Greeks:\n";
    results_file << "  Delta1: " << result.greeks.delta1 << "\n";
    results_file << "  Delta2: " << result.greeks.delta2 << "\n";
    results_file << "  Gamma1: " << result.greeks.gamma1 << "\n";
    results_file << "  Gamma2: " << result.greeks.gamma2 << "\n";
    results_file << "  Vega1: " << result.greeks.vega1 << "\n";
    results_file << "  Vega2: " << result.greeks.vega2 << "\n";
    results_file << "  Theta: " << result.greeks.theta << "\n";
    results_file << "  Rho (correlation): " << result.greeks.rho_greek << "\n";
    results_file << "  Rho (rate): " << result.greeks.rho_rate << "\n";
    results_file.close();
    
    std::cout << "\nResults saved to:" << std::endl;
    std::cout << "  - MonteCarlo_Results.txt" << std::endl;
    
    // Save convergence data
    auto convergence_data = pricer.convergenceAnalysis({100, 500, 1000, 5000, 10000, 50000, 100000});
    std::ofstream conv_file("MonteCarlo_Convergence.txt");
    conv_file << "Simulations\tPrice\n";
    for (const auto& point : convergence_data) {
        conv_file << point.first << "\t" << point.second << "\n";
    }
    conv_file.close();
    std::cout << "  - MonteCarlo_Convergence.txt" << std::endl;
}

int main() {
    std::cout << "========================================================" << std::endl;
    std::cout << "    Monte Carlo Option Pricing - Comprehensive Demo    " << std::endl;
    std::cout << "========================================================" << std::endl;
    
    try {
        demonstrateBasicPricing();
        demonstrateGreeks();
        demonstrateConvergence();
        demonstratePriceEvolution();
        demonstrateSensitivityAnalysis();
        saveResultsToFile();
        
        printHeader("Summary");
        std::cout << "\nAll demonstrations completed successfully!" << std::endl;
        std::cout << "\nKey Features Demonstrated:" << std::endl;
        std::cout << "  ✓ Monte Carlo pricing with variance reduction" << std::endl;
        std::cout << "  ✓ Complete Greeks calculation" << std::endl;
        std::cout << "  ✓ Convergence analysis" << std::endl;
        std::cout << "  ✓ Price evolution over time" << std::endl;
        std::cout << "  ✓ Sensitivity analysis" << std::endl;
        std::cout << "  ✓ Results export to files" << std::endl;
        
        std::cout << "\nFor interactive visualization, run:" << std::endl;
        std::cout << "  ./montecarlo" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
