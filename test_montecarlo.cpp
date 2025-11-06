#include <iostream>
#include <iomanip>
#include <chrono>
#include "monteCarlo.hpp"

int main() {
    std::cout << "=== Simple Monte Carlo Test ===" << std::endl;
    
    // Set up option parameters matching the FEM implementation
    OptionParams params;
    params.S1_0 = 1.0;
    params.S2_0 = 1.0;
    params.K = 1.0;
    params.T = 2.0;  // 2 years
    params.r = 0.05;
    
    // sigma1 = sigma2 = sqrt(0.04) = 0.2
    params.sigma1 = 0.2;
    params.sigma2 = 0.2;
    
    // correlation from covariance matrix: rho = -0.024 / (0.2 * 0.2) = -0.6
    params.rho = -0.6;
    
    std::cout << "\nOption Parameters:" << std::endl;
    std::cout << "  S1(0) = " << params.S1_0 << std::endl;
    std::cout << "  S2(0) = " << params.S2_0 << std::endl;
    std::cout << "  Strike K = " << params.K << std::endl;
    std::cout << "  Maturity T = " << params.T << " years" << std::endl;
    std::cout << "  Risk-free rate r = " << params.r << std::endl;
    std::cout << "  Volatility σ1 = " << params.sigma1 << std::endl;
    std::cout << "  Volatility σ2 = " << params.sigma2 << std::endl;
    std::cout << "  Correlation ρ = " << params.rho << std::endl;
    
    // Test with different simulation counts
    std::cout << "\n=== Convergence Test ===" << std::endl;
    std::cout << std::setw(12) << "Simulations" 
              << std::setw(15) << "Price" 
              << std::setw(15) << "Std Error" 
              << std::setw(10) << "Time (s)" << std::endl;
    std::cout << std::string(52, '-') << std::endl;
    
    std::vector<int> sim_counts = {1000, 5000, 10000, 50000, 100000};
    
    for (int num_sims : sim_counts) {
        auto start = std::chrono::high_resolution_clock::now();
        
        MonteCarloPricer pricer(params, num_sims, 42);
        auto result = pricer.priceOptionAntithetic();
        
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        std::cout << std::setw(12) << num_sims
                  << std::setw(15) << std::setprecision(6) << result.price
                  << std::setw(15) << std::setprecision(6) << result.std_error
                  << std::setw(10) << std::setprecision(3) << elapsed << std::endl;
    }
    
    // Detailed Greeks calculation
    std::cout << "\n=== Greeks Calculation (100,000 simulations) ===" << std::endl;
    
    MonteCarloPricer pricer(params, 100000, 42);
    auto result = pricer.priceOptionAntithetic();
    
    std::cout << "\nOption Value:" << std::endl;
    std::cout << "  Price: " << std::setprecision(6) << result.price 
              << " ± " << result.std_error << std::endl;
    
    std::cout << "\nFirst-Order Greeks:" << std::endl;
    std::cout << "  Delta1 (∂V/∂S1): " << std::setprecision(6) << result.greeks.delta1 << std::endl;
    std::cout << "  Delta2 (∂V/∂S2): " << result.greeks.delta2 << std::endl;
    std::cout << "  Rho_rate (∂V/∂r): " << result.greeks.rho_rate << std::endl;
    
    std::cout << "\nSecond-Order Greeks:" << std::endl;
    std::cout << "  Gamma1 (∂²V/∂S1²): " << result.greeks.gamma1 << std::endl;
    std::cout << "  Gamma2 (∂²V/∂S2²): " << result.greeks.gamma2 << std::endl;
    
    std::cout << "\nVolatility Greeks:" << std::endl;
    std::cout << "  Vega1 (∂V/∂σ1): " << result.greeks.vega1 << std::endl;
    std::cout << "  Vega2 (∂V/∂σ2): " << result.greeks.vega2 << std::endl;
    
    std::cout << "\nOther Greeks:" << std::endl;
    std::cout << "  Theta (∂V/∂t): " << result.greeks.theta << " (per year)" << std::endl;
    std::cout << "  Theta: " << result.greeks.theta / 365.0 << " (per day)" << std::endl;
    std::cout << "  Rho_corr (∂V/∂ρ): " << result.greeks.rho_greek << std::endl;
    
    // Interpretation
    std::cout << "\n=== Interpretation ===" << std::endl;
    std::cout << "For a $1 increase in:" << std::endl;
    std::cout << "  Asset 1 price → Option value changes by $" 
              << std::setprecision(4) << result.greeks.delta1 << std::endl;
    std::cout << "  Asset 2 price → Option value changes by $" 
              << result.greeks.delta2 << std::endl;
    
    std::cout << "\nRisk measures:" << std::endl;
    std::cout << "  Convexity (Gamma1): " << result.greeks.gamma1 << std::endl;
    std::cout << "  Time decay per day: $" << -result.greeks.theta / 365.0 << std::endl;
    
    std::cout << "\nTest completed successfully!" << std::endl;
    
    return 0;
}
