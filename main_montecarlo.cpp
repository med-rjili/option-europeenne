#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "monteCarlo.hpp"
#include "monteCarloVisualization.hpp"

int main() {
    std::cout << "=== European Option Pricing with Monte Carlo ===" << std::endl;
    std::cout << "\nThis program prices a European call option on two correlated assets" << std::endl;
    std::cout << "Payoff: max(S1 + S2 - K, 0)" << std::endl;
    
    // Set up option parameters
    OptionParams params;
    
    std::cout << "\n--- Option Parameters ---" << std::endl;
    std::cout << "Enter initial price of asset 1 (default 1.0): ";
    std::string input;
    std::getline(std::cin, input);
    params.S1_0 = input.empty() ? 1.0 : std::stod(input);
    
    std::cout << "Enter initial price of asset 2 (default 1.0): ";
    std::getline(std::cin, input);
    params.S2_0 = input.empty() ? 1.0 : std::stod(input);
    
    std::cout << "Enter strike price K (default 1.0): ";
    std::getline(std::cin, input);
    params.K = input.empty() ? 1.0 : std::stod(input);
    
    std::cout << "Enter time to maturity in years (default 2.0): ";
    std::getline(std::cin, input);
    params.T = input.empty() ? 2.0 : std::stod(input);
    
    std::cout << "Enter risk-free rate (default 0.05): ";
    std::getline(std::cin, input);
    params.r = input.empty() ? 0.05 : std::stod(input);
    
    std::cout << "Enter volatility of asset 1 (default 0.2): ";
    std::getline(std::cin, input);
    params.sigma1 = input.empty() ? 0.2 : std::stod(input);
    
    std::cout << "Enter volatility of asset 2 (default 0.2): ";
    std::getline(std::cin, input);
    params.sigma2 = input.empty() ? 0.2 : std::stod(input);
    
    std::cout << "Enter correlation between assets (default -0.6, range [-1, 1]): ";
    std::getline(std::cin, input);
    params.rho = input.empty() ? -0.6 : std::stod(input);
    if (params.rho < -1.0 || params.rho > 1.0) {
        std::cout << "Warning: Correlation must be in [-1, 1]. Using -0.6" << std::endl;
        params.rho = -0.6;
    }
    
    std::cout << "\nEnter number of Monte Carlo simulations (default 100000): ";
    std::getline(std::cin, input);
    int num_simulations = input.empty() ? 100000 : std::stoi(input);
    
    // Run the visualization
    runMonteCarloVisualization(params, num_simulations);
    
    return 0;
}
