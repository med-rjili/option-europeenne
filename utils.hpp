#ifndef UTILS_HPP
#define UTILS_HPP
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
void drawLine(sf::RenderWindow& window, int x1, int y1, int x2, int y2, sf::Color color, float thickness);
double max(std::vector<double> &v);
#endif