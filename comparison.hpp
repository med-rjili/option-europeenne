#ifndef COMPARISON_HPP
#define COMPARISON_HPP

#include <vector>
#include <string>
#include "monteCarlo.hpp"

// Structure to hold comparison results
struct ComparisonResult {
    std::vector<double> time_points;
    std::vector<double> mc_prices;
    std::vector<double> fem_prices;
    std::vector<double> mc_std_errors;
    
    // Greeks comparison
    Greeks mc_greeks;
    
    // Performance metrics
    double mc_time_seconds;
    double fem_time_seconds;
    
    // Convergence data
    std::vector<std::pair<int, double>> convergence_data;
};

// Function to extract FEM prices from Solution.txt
std::vector<double> extractFEMPrices(const std::string& filename, 
                                     const std::vector<double>& time_points,
                                     int point_index = 0);

// Function to run Monte Carlo at multiple time points
std::vector<double> runMonteCarloAtTimePoints(const OptionParams& params,
                                              const std::vector<double>& time_points,
                                              int num_simulations);

// Main comparison function
ComparisonResult compareMethodsFEMvsMC(const OptionParams& params,
                                       int num_simulations,
                                       const std::string& fem_solution_file);

// Save comparison results to file
void saveComparisonResults(const ComparisonResult& result, 
                          const std::string& filename);

// Print comparison summary
void printComparisonSummary(const ComparisonResult& result);

#endif // COMPARISON_HPP
