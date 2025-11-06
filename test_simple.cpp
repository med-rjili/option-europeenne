#include <iostream>
#include <iomanip>
#include "monteCarlo.hpp"

int main() {
    std::cout << "=== Basic Monte Carlo Test ===" << std::endl;
    
    OptionParams params;
    params.S1_0 = 1.0;
    params.S2_0 = 1.0;
    params.K = 1.0;
    params.T = 2.0;
    params.r = 0.05;
    params.sigma1 = 0.2;
    params.sigma2 = 0.2;
    params.rho = -0.6;
    
    std::cout << "Creating pricer..." << std::endl;
    int num_sims = 1000;
    MonteCarloPricer pricer(params, num_sims, 42);
    
    std::cout << "Pricing option (simple)..." << std::endl;
    auto result = pricer.priceOption();
    
    std::cout << "Price: " << result.price << std::endl;
    std::cout << "Std Error: " << result.std_error << std::endl;
    
    std::cout << "\nTest completed!" << std::endl;
    
    return 0;
}
