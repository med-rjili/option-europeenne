#include "utils.hpp"


double max(std::vector<double> &v){
    double maxi = abs(v[0]);
    for(int i=0;i<v.size();i++){
        if (abs(v[i])> maxi){
            maxi = abs(v[i]);
        }
    }
    return maxi;
}

void drawLine(sf::RenderWindow& window, int x1, int y1, int x2, int y2, sf::Color color, float thickness)
{
    // Calculate the length of the line
    float length = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    // Calculate the angle of the line
    float angle = std::atan2(y2 - y1, x2 - x1) * 180 / M_PI;

    // Create a rectangle with the given thickness and length
    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setFillColor(color);

    // Set the origin of the rectangle to the left side (0, thickness / 2)
    line.setOrigin(0, thickness / 2);

    // Position and rotate the rectangle to match the line
    line.setPosition(x1, y1);
    line.setRotation(angle);

    window.draw(line);
}

