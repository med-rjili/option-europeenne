#include "monteCarloVisualization.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

void drawAxes(sf::RenderWindow& window, 
             double x_min, double x_max, 
             double y_min, double y_max,
             const std::string& x_label,
             const std::string& y_label,
             sf::Font& font) {
    
    const float margin_left = 80;
    const float margin_right = 50;
    const float margin_top = 50;
    const float margin_bottom = 80;
    
    float plot_width = MC_WIDTH - margin_left - margin_right;
    float plot_height = MC_HEIGHT - margin_top - margin_bottom;
    
    // Draw axes
    sf::Vertex x_axis[] = {
        sf::Vertex(sf::Vector2f(margin_left, MC_HEIGHT - margin_bottom), sf::Color::White),
        sf::Vertex(sf::Vector2f(MC_WIDTH - margin_right, MC_HEIGHT - margin_bottom), sf::Color::White)
    };
    window.draw(x_axis, 2, sf::Lines);
    
    sf::Vertex y_axis[] = {
        sf::Vertex(sf::Vector2f(margin_left, margin_top), sf::Color::White),
        sf::Vertex(sf::Vector2f(margin_left, MC_HEIGHT - margin_bottom), sf::Color::White)
    };
    window.draw(y_axis, 2, sf::Lines);
    
    // X-axis label
    sf::Text x_text(x_label, font, 14);
    x_text.setPosition(MC_WIDTH / 2 - 50, MC_HEIGHT - margin_bottom + 30);
    x_text.setFillColor(sf::Color::White);
    window.draw(x_text);
    
    // Y-axis label
    sf::Text y_text(y_label, font, 14);
    y_text.setPosition(10, MC_HEIGHT / 2 - 50);
    y_text.setFillColor(sf::Color::White);
    window.draw(y_text);
    
    // Draw tick marks and labels
    const int num_ticks = 5;
    for (int i = 0; i <= num_ticks; ++i) {
        // X-axis ticks
        float x_pos = margin_left + (plot_width * i / num_ticks);
        double x_val = x_min + (x_max - x_min) * i / num_ticks;
        
        sf::Vertex x_tick[] = {
            sf::Vertex(sf::Vector2f(x_pos, MC_HEIGHT - margin_bottom - 5), sf::Color::White),
            sf::Vertex(sf::Vector2f(x_pos, MC_HEIGHT - margin_bottom + 5), sf::Color::White)
        };
        window.draw(x_tick, 2, sf::Lines);
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << x_val;
        sf::Text tick_label(oss.str(), font, 10);
        tick_label.setPosition(x_pos - 15, MC_HEIGHT - margin_bottom + 10);
        tick_label.setFillColor(sf::Color::White);
        window.draw(tick_label);
        
        // Y-axis ticks
        float y_pos = MC_HEIGHT - margin_bottom - (plot_height * i / num_ticks);
        double y_val = y_min + (y_max - y_min) * i / num_ticks;
        
        sf::Vertex y_tick[] = {
            sf::Vertex(sf::Vector2f(margin_left - 5, y_pos), sf::Color::White),
            sf::Vertex(sf::Vector2f(margin_left + 5, y_pos), sf::Color::White)
        };
        window.draw(y_tick, 2, sf::Lines);
        
        std::ostringstream oss_y;
        oss_y << std::fixed << std::setprecision(2) << y_val;
        sf::Text tick_label_y(oss_y.str(), font, 10);
        tick_label_y.setPosition(margin_left - 60, y_pos - 7);
        tick_label_y.setFillColor(sf::Color::White);
        window.draw(tick_label_y);
    }
}

void plotConvergence(sf::RenderWindow& window, 
                    const std::vector<std::pair<int, double>>& convergence_data,
                    const std::string& title,
                    sf::Font& font) {
    
    window.clear(sf::Color::Black);
    
    // Title
    sf::Text title_text(title, font, 20);
    title_text.setPosition(MC_WIDTH / 2 - 150, 10);
    title_text.setFillColor(sf::Color::White);
    window.draw(title_text);
    
    if (convergence_data.empty()) {
        window.display();
        return;
    }
    
    // Find min/max for scaling
    double max_sims = convergence_data.back().first;
    double min_price = convergence_data[0].second;
    double max_price = convergence_data[0].second;
    
    for (const auto& point : convergence_data) {
        min_price = std::min(min_price, point.second);
        max_price = std::max(max_price, point.second);
    }
    
    // Add some padding
    double price_range = max_price - min_price;
    min_price -= price_range * 0.1;
    max_price += price_range * 0.1;
    
    drawAxes(window, 0, max_sims, min_price, max_price, 
             "Number of Simulations", "Option Price", font);
    
    // Plot data points
    const float margin_left = 80;
    const float margin_bottom = 80;
    const float margin_top = 50;
    const float margin_right = 50;
    float plot_width = MC_WIDTH - margin_left - margin_right;
    float plot_height = MC_HEIGHT - margin_top - margin_bottom;
    
    for (size_t i = 0; i < convergence_data.size(); ++i) {
        double x = convergence_data[i].first;
        double y = convergence_data[i].second;
        
        float screen_x = margin_left + (x / max_sims) * plot_width;
        float screen_y = MC_HEIGHT - margin_bottom - ((y - min_price) / (max_price - min_price)) * plot_height;
        
        sf::CircleShape point(3);
        point.setPosition(screen_x - 3, screen_y - 3);
        point.setFillColor(sf::Color::Red);
        window.draw(point);
        
        // Draw line to next point
        if (i < convergence_data.size() - 1) {
            double next_x = convergence_data[i + 1].first;
            double next_y = convergence_data[i + 1].second;
            
            float next_screen_x = margin_left + (next_x / max_sims) * plot_width;
            float next_screen_y = MC_HEIGHT - margin_bottom - ((next_y - min_price) / (max_price - min_price)) * plot_height;
            
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Cyan),
                sf::Vertex(sf::Vector2f(next_screen_x, next_screen_y), sf::Color::Cyan)
            };
            window.draw(line, 2, sf::Lines);
        }
    }
    
    window.display();
}

void plotGreeks(sf::RenderWindow& window,
               const std::vector<double>& S_values,
               const std::vector<double>& greek_values,
               const std::string& greek_name,
               sf::Font& font) {
    
    window.clear(sf::Color::Black);
    
    // Title
    std::string title = "Greek: " + greek_name;
    sf::Text title_text(title, font, 20);
    title_text.setPosition(MC_WIDTH / 2 - 100, 10);
    title_text.setFillColor(sf::Color::White);
    window.draw(title_text);
    
    if (S_values.empty() || greek_values.empty()) {
        window.display();
        return;
    }
    
    // Find min/max for scaling
    double min_S = *std::min_element(S_values.begin(), S_values.end());
    double max_S = *std::max_element(S_values.begin(), S_values.end());
    double min_greek = *std::min_element(greek_values.begin(), greek_values.end());
    double max_greek = *std::max_element(greek_values.begin(), greek_values.end());
    
    // Add some padding
    double greek_range = max_greek - min_greek;
    if (greek_range < 1e-10) {
        min_greek -= 0.1;
        max_greek += 0.1;
    } else {
        min_greek -= greek_range * 0.1;
        max_greek += greek_range * 0.1;
    }
    
    drawAxes(window, min_S, max_S, min_greek, max_greek, 
             "Spot Price", greek_name, font);
    
    // Plot data points
    const float margin_left = 80;
    const float margin_bottom = 80;
    const float margin_top = 50;
    const float margin_right = 50;
    float plot_width = MC_WIDTH - margin_left - margin_right;
    float plot_height = MC_HEIGHT - margin_top - margin_bottom;
    
    for (size_t i = 0; i < S_values.size(); ++i) {
        double x = S_values[i];
        double y = greek_values[i];
        
        float screen_x = margin_left + ((x - min_S) / (max_S - min_S)) * plot_width;
        float screen_y = MC_HEIGHT - margin_bottom - ((y - min_greek) / (max_greek - min_greek)) * plot_height;
        
        sf::CircleShape point(3);
        point.setPosition(screen_x - 3, screen_y - 3);
        point.setFillColor(sf::Color::Green);
        window.draw(point);
        
        // Draw line to next point
        if (i < S_values.size() - 1) {
            double next_x = S_values[i + 1];
            double next_y = greek_values[i + 1];
            
            float next_screen_x = margin_left + ((next_x - min_S) / (max_S - min_S)) * plot_width;
            float next_screen_y = MC_HEIGHT - margin_bottom - ((next_y - min_greek) / (max_greek - min_greek)) * plot_height;
            
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Yellow),
                sf::Vertex(sf::Vector2f(next_screen_x, next_screen_y), sf::Color::Yellow)
            };
            window.draw(line, 2, sf::Lines);
        }
    }
    
    window.display();
}

void plotPriceEvolution(sf::RenderWindow& window,
                       const std::vector<double>& times,
                       const std::vector<double>& prices,
                       const std::string& title,
                       sf::Font& font) {
    
    window.clear(sf::Color::Black);
    
    // Title
    sf::Text title_text(title, font, 20);
    title_text.setPosition(MC_WIDTH / 2 - 150, 10);
    title_text.setFillColor(sf::Color::White);
    window.draw(title_text);
    
    if (times.empty() || prices.empty()) {
        window.display();
        return;
    }
    
    // Find min/max for scaling
    double max_time = *std::max_element(times.begin(), times.end());
    double min_price = *std::min_element(prices.begin(), prices.end());
    double max_price = *std::max_element(prices.begin(), prices.end());
    
    // Add some padding
    double price_range = max_price - min_price;
    if (price_range < 1e-10) {
        min_price -= 0.1;
        max_price += 0.1;
    } else {
        min_price -= price_range * 0.1;
        max_price += price_range * 0.1;
    }
    
    drawAxes(window, 0, max_time, min_price, max_price, 
             "Time to Maturity (years)", "Option Price", font);
    
    // Plot data points
    const float margin_left = 80;
    const float margin_bottom = 80;
    const float margin_top = 50;
    const float margin_right = 50;
    float plot_width = MC_WIDTH - margin_left - margin_right;
    float plot_height = MC_HEIGHT - margin_top - margin_bottom;
    
    for (size_t i = 0; i < times.size(); ++i) {
        double x = times[i];
        double y = prices[i];
        
        float screen_x = margin_left + (x / max_time) * plot_width;
        float screen_y = MC_HEIGHT - margin_bottom - ((y - min_price) / (max_price - min_price)) * plot_height;
        
        sf::CircleShape point(3);
        point.setPosition(screen_x - 3, screen_y - 3);
        point.setFillColor(sf::Color::Magenta);
        window.draw(point);
        
        // Draw line to next point
        if (i < times.size() - 1) {
            double next_x = times[i + 1];
            double next_y = prices[i + 1];
            
            float next_screen_x = margin_left + (next_x / max_time) * plot_width;
            float next_screen_y = MC_HEIGHT - margin_bottom - ((next_y - min_price) / (max_price - min_price)) * plot_height;
            
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Cyan),
                sf::Vertex(sf::Vector2f(next_screen_x, next_screen_y), sf::Color::Cyan)
            };
            window.draw(line, 2, sf::Lines);
        }
    }
    
    window.display();
}

void plotComparisonMCvsFEM(sf::RenderWindow& window,
                          const std::vector<double>& times,
                          const std::vector<double>& mc_prices,
                          const std::vector<double>& fem_prices,
                          sf::Font& font) {
    
    window.clear(sf::Color::Black);
    
    // Title
    sf::Text title_text("Monte Carlo vs Finite Elements", font, 20);
    title_text.setPosition(MC_WIDTH / 2 - 150, 10);
    title_text.setFillColor(sf::Color::White);
    window.draw(title_text);
    
    if (times.empty() || mc_prices.empty() || fem_prices.empty()) {
        window.display();
        return;
    }
    
    // Find min/max for scaling
    double max_time = *std::max_element(times.begin(), times.end());
    double min_price = std::min(*std::min_element(mc_prices.begin(), mc_prices.end()),
                                *std::min_element(fem_prices.begin(), fem_prices.end()));
    double max_price = std::max(*std::max_element(mc_prices.begin(), mc_prices.end()),
                                *std::max_element(fem_prices.begin(), fem_prices.end()));
    
    // Add some padding
    double price_range = max_price - min_price;
    min_price -= price_range * 0.1;
    max_price += price_range * 0.1;
    
    drawAxes(window, 0, max_time, min_price, max_price, 
             "Time (days)", "Option Price", font);
    
    // Plot data points
    const float margin_left = 80;
    const float margin_bottom = 80;
    const float margin_top = 50;
    const float margin_right = 50;
    float plot_width = MC_WIDTH - margin_left - margin_right;
    float plot_height = MC_HEIGHT - margin_top - margin_bottom;
    
    // Plot Monte Carlo prices
    for (size_t i = 0; i < times.size(); ++i) {
        double x = times[i];
        double y = mc_prices[i];
        
        float screen_x = margin_left + (x / max_time) * plot_width;
        float screen_y = MC_HEIGHT - margin_bottom - ((y - min_price) / (max_price - min_price)) * plot_height;
        
        if (i < times.size() - 1) {
            double next_x = times[i + 1];
            double next_y = mc_prices[i + 1];
            
            float next_screen_x = margin_left + (next_x / max_time) * plot_width;
            float next_screen_y = MC_HEIGHT - margin_bottom - ((next_y - min_price) / (max_price - min_price)) * plot_height;
            
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Red),
                sf::Vertex(sf::Vector2f(next_screen_x, next_screen_y), sf::Color::Red)
            };
            window.draw(line, 2, sf::Lines);
        }
    }
    
    // Plot FEM prices
    for (size_t i = 0; i < times.size(); ++i) {
        double x = times[i];
        double y = fem_prices[i];
        
        float screen_x = margin_left + (x / max_time) * plot_width;
        float screen_y = MC_HEIGHT - margin_bottom - ((y - min_price) / (max_price - min_price)) * plot_height;
        
        if (i < times.size() - 1) {
            double next_x = times[i + 1];
            double next_y = fem_prices[i + 1];
            
            float next_screen_x = margin_left + (next_x / max_time) * plot_width;
            float next_screen_y = MC_HEIGHT - margin_bottom - ((next_y - min_price) / (max_price - min_price)) * plot_height;
            
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Green),
                sf::Vertex(sf::Vector2f(next_screen_x, next_screen_y), sf::Color::Green)
            };
            window.draw(line, 2, sf::Lines);
        }
    }
    
    // Draw legend
    std::vector<std::string> labels = {"Monte Carlo", "Finite Elements"};
    std::vector<sf::Color> colors = {sf::Color::Red, sf::Color::Green};
    drawLegend(window, labels, colors, font);
    
    window.display();
}

void drawLegend(sf::RenderWindow& window,
               const std::vector<std::string>& labels,
               const std::vector<sf::Color>& colors,
               sf::Font& font) {
    
    const float legend_x = MC_WIDTH - 200;
    const float legend_y = 100;
    const float line_height = 25;
    
    for (size_t i = 0; i < labels.size(); ++i) {
        // Draw colored line
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(legend_x, legend_y + i * line_height), colors[i]),
            sf::Vertex(sf::Vector2f(legend_x + 30, legend_y + i * line_height), colors[i])
        };
        window.draw(line, 2, sf::Lines);
        
        // Draw label
        sf::Text label_text(labels[i], font, 12);
        label_text.setPosition(legend_x + 40, legend_y + i * line_height - 10);
        label_text.setFillColor(sf::Color::White);
        window.draw(label_text);
    }
}

sf::Color valueToColor(double value, double min_val, double max_val) {
    // Map value to a color gradient (blue -> green -> yellow -> red)
    double normalized = (value - min_val) / (max_val - min_val);
    normalized = std::max(0.0, std::min(1.0, normalized));
    
    if (normalized < 0.25) {
        // Blue to Cyan
        double t = normalized * 4;
        return sf::Color(0, static_cast<sf::Uint8>(t * 255), 255);
    } else if (normalized < 0.5) {
        // Cyan to Green
        double t = (normalized - 0.25) * 4;
        return sf::Color(0, 255, static_cast<sf::Uint8>((1 - t) * 255));
    } else if (normalized < 0.75) {
        // Green to Yellow
        double t = (normalized - 0.5) * 4;
        return sf::Color(static_cast<sf::Uint8>(t * 255), 255, 0);
    } else {
        // Yellow to Red
        double t = (normalized - 0.75) * 4;
        return sf::Color(255, static_cast<sf::Uint8>((1 - t) * 255), 0);
    }
}

void plotHeatmap(sf::RenderWindow& window,
                const std::vector<double>& S1_values,
                const std::vector<double>& S2_values,
                const std::vector<double>& prices,
                const std::string& title,
                sf::Font& font) {
    
    window.clear(sf::Color::Black);
    
    // Title
    sf::Text title_text(title, font, 20);
    title_text.setPosition(MC_WIDTH / 2 - 150, 10);
    title_text.setFillColor(sf::Color::White);
    window.draw(title_text);
    
    if (S1_values.empty() || S2_values.empty() || prices.empty()) {
        window.display();
        return;
    }
    
    // Find min/max for color scaling
    double min_price = *std::min_element(prices.begin(), prices.end());
    double max_price = *std::max_element(prices.begin(), prices.end());
    
    const float margin_left = 80;
    const float margin_bottom = 80;
    const float margin_top = 50;
    const float margin_right = 100;
    float plot_width = MC_WIDTH - margin_left - margin_right;
    float plot_height = MC_HEIGHT - margin_top - margin_bottom;
    
    float cell_width = plot_width / S1_values.size();
    float cell_height = plot_height / S2_values.size();
    
    // Draw heatmap cells
    for (size_t i = 0; i < S1_values.size(); ++i) {
        for (size_t j = 0; j < S2_values.size(); ++j) {
            size_t idx = i * S2_values.size() + j;
            if (idx >= prices.size()) continue;
            
            double price = prices[idx];
            sf::Color color = valueToColor(price, min_price, max_price);
            
            sf::RectangleShape cell(sf::Vector2f(cell_width, cell_height));
            cell.setPosition(margin_left + i * cell_width, 
                           margin_top + (S2_values.size() - 1 - j) * cell_height);
            cell.setFillColor(color);
            window.draw(cell);
        }
    }
    
    // Draw axes labels
    sf::Text x_label("Asset 1 Price", font, 14);
    x_label.setPosition(MC_WIDTH / 2 - 50, MC_HEIGHT - margin_bottom + 30);
    x_label.setFillColor(sf::Color::White);
    window.draw(x_label);
    
    sf::Text y_label("Asset 2 Price", font, 14);
    y_label.setPosition(10, MC_HEIGHT / 2 - 50);
    y_label.setFillColor(sf::Color::White);
    window.draw(y_label);
    
    // Draw color bar
    const int color_bar_width = 30;
    const int color_bar_height = static_cast<int>(plot_height);
    const int num_color_steps = 100;
    
    for (int i = 0; i < num_color_steps; ++i) {
        double value = min_price + (max_price - min_price) * i / num_color_steps;
        sf::Color color = valueToColor(value, min_price, max_price);
        
        sf::RectangleShape bar_segment(sf::Vector2f(color_bar_width, 
                                                     color_bar_height / num_color_steps));
        bar_segment.setPosition(MC_WIDTH - margin_right + 20, 
                               margin_top + (num_color_steps - 1 - i) * color_bar_height / num_color_steps);
        bar_segment.setFillColor(color);
        window.draw(bar_segment);
    }
    
    // Color bar labels
    std::ostringstream oss_max, oss_min;
    oss_max << std::fixed << std::setprecision(2) << max_price;
    oss_min << std::fixed << std::setprecision(2) << min_price;
    
    sf::Text max_label(oss_max.str(), font, 10);
    max_label.setPosition(MC_WIDTH - margin_right + 55, margin_top);
    max_label.setFillColor(sf::Color::White);
    window.draw(max_label);
    
    sf::Text min_label(oss_min.str(), font, 10);
    min_label.setPosition(MC_WIDTH - margin_right + 55, margin_top + color_bar_height - 10);
    min_label.setFillColor(sf::Color::White);
    window.draw(min_label);
    
    window.display();
}

void runMonteCarloVisualization(const OptionParams& params, int num_simulations) {
    sf::RenderWindow window(sf::VideoMode(MC_WIDTH, MC_HEIGHT), "Monte Carlo Option Pricing");
    
    sf::Font font;
    // Try multiple font paths for robustness
    std::vector<std::string> font_paths = {
        "arial.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Helvetica.ttc",  // macOS
        "C:\\Windows\\Fonts\\arial.ttf"  // Windows
    };
    
    bool font_loaded = false;
    for (const auto& path : font_paths) {
        if (font.loadFromFile(path)) {
            font_loaded = true;
            std::cout << "Loaded font from: " << path << std::endl;
            break;
        }
    }
    
    if (!font_loaded) {
        std::cerr << "Error: Could not load any font. Tried:" << std::endl;
        for (const auto& path : font_paths) {
            std::cerr << "  - " << path << std::endl;
        }
        std::cerr << "Visualization will not work properly without fonts." << std::endl;
        return;
    }
    
    std::cout << "\n=== Monte Carlo Option Pricing ===" << std::endl;
    std::cout << "Parameters:" << std::endl;
    std::cout << "  S1_0: " << params.S1_0 << std::endl;
    std::cout << "  S2_0: " << params.S2_0 << std::endl;
    std::cout << "  K: " << params.K << std::endl;
    std::cout << "  T: " << params.T << " years" << std::endl;
    std::cout << "  r: " << params.r << std::endl;
    std::cout << "  sigma1: " << params.sigma1 << std::endl;
    std::cout << "  sigma2: " << params.sigma2 << std::endl;
    std::cout << "  rho: " << params.rho << std::endl;
    std::cout << "  Simulations: " << num_simulations << std::endl;
    
    // Price the option
    std::cout << "\nPricing option..." << std::endl;
    MonteCarloPricer pricer(params, num_simulations, 42);
    MonteCarloResult result = pricer.priceOptionAntithetic();
    
    std::cout << "\nResults:" << std::endl;
    std::cout << "  Option Price: " << result.price << " +/- " << result.std_error << std::endl;
    std::cout << "\nGreeks:" << std::endl;
    std::cout << "  Delta1: " << result.greeks.delta1 << std::endl;
    std::cout << "  Delta2: " << result.greeks.delta2 << std::endl;
    std::cout << "  Gamma1: " << result.greeks.gamma1 << std::endl;
    std::cout << "  Gamma2: " << result.greeks.gamma2 << std::endl;
    std::cout << "  Vega1: " << result.greeks.vega1 << std::endl;
    std::cout << "  Vega2: " << result.greeks.vega2 << std::endl;
    std::cout << "  Rho (correlation): " << result.greeks.rho_greek << std::endl;
    std::cout << "  Theta: " << result.greeks.theta << std::endl;
    std::cout << "  Rho (rate): " << result.greeks.rho_rate << std::endl;
    
    // Convergence analysis
    std::cout << "\nPerforming convergence analysis..." << std::endl;
    std::vector<int> sim_counts = {100, 500, 1000, 2000, 5000, 10000, 20000, 50000};
    auto convergence = pricer.convergenceAnalysis(sim_counts);
    
    plotConvergence(window, convergence, "Monte Carlo Convergence", font);
    
    // Wait for user input before next plot
    bool next_plot = false;
    while (window.isOpen() && !next_plot) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
                next_plot = true;
            }
        }
    }
    
    // Price evolution
    std::cout << "\nCalculating price evolution..." << std::endl;
    int num_time_steps = 20;
    auto prices_evolution = pricer.priceEvolution(num_time_steps);
    std::vector<double> times;
    for (int i = 0; i < num_time_steps; ++i) {
        times.push_back(params.T * (i + 1) / num_time_steps);
    }
    
    plotPriceEvolution(window, times, prices_evolution, "Option Price Evolution", font);
    
    // Wait for user input before next plot
    next_plot = false;
    while (window.isOpen() && !next_plot) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
                next_plot = true;
            }
        }
    }
    
    // Greek sensitivity - Delta 1
    std::cout << "\nCalculating Delta1 sensitivity..." << std::endl;
    std::vector<double> S1_values;
    std::vector<double> delta1_values;
    
    for (double S1 = params.S1_0 * 0.5; S1 <= params.S1_0 * 1.5; S1 += params.S1_0 * 0.1) {
        OptionParams temp_params = params;
        temp_params.S1_0 = S1;
        MonteCarloPricer temp_pricer(temp_params, 10000, 42);
        auto temp_result = temp_pricer.priceOption();
        S1_values.push_back(S1);
        delta1_values.push_back(temp_result.greeks.delta1);
    }
    
    plotGreeks(window, S1_values, delta1_values, "Delta (Asset 1)", font);
    
    // Wait for user input before closing
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
}
