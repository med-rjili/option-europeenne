#ifndef MONTE_CARLO_VISUALIZATION_HPP
#define MONTE_CARLO_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "monteCarlo.hpp"

// Window dimensions
const int MC_WIDTH = 1200;
const int MC_HEIGHT = 900;

// Visualization functions
void plotConvergence(sf::RenderWindow& window, 
                    const std::vector<std::pair<int, double>>& convergence_data,
                    const std::string& title,
                    sf::Font& font);

void plotGreeks(sf::RenderWindow& window,
               const std::vector<double>& S_values,
               const std::vector<double>& greek_values,
               const std::string& greek_name,
               sf::Font& font);

void plotPriceEvolution(sf::RenderWindow& window,
                       const std::vector<double>& times,
                       const std::vector<double>& prices,
                       const std::string& title,
                       sf::Font& font);

void plotComparisonMCvsFEM(sf::RenderWindow& window,
                          const std::vector<double>& times,
                          const std::vector<double>& mc_prices,
                          const std::vector<double>& fem_prices,
                          sf::Font& font);

void plotHeatmap(sf::RenderWindow& window,
                const std::vector<double>& S1_values,
                const std::vector<double>& S2_values,
                const std::vector<double>& prices,
                const std::string& title,
                sf::Font& font);

// Utility functions
void drawAxes(sf::RenderWindow& window, 
             double x_min, double x_max, 
             double y_min, double y_max,
             const std::string& x_label,
             const std::string& y_label,
             sf::Font& font);

void drawLegend(sf::RenderWindow& window,
               const std::vector<std::string>& labels,
               const std::vector<sf::Color>& colors,
               sf::Font& font);

sf::Color valueToColor(double value, double min_val, double max_val);

// Main Monte Carlo visualization interface
void runMonteCarloVisualization(const OptionParams& params, int num_simulations);

#endif // MONTE_CARLO_VISUALIZATION_HPP
