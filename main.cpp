#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <bits/stdc++.h>
#include<string>
#include "maillage.hpp"
#include "elementsFinies.hpp"
#include "vecteur.hpp"
#include "utils.hpp"
#include "pricing.hpp"

const double HEIGHT=900;
const double WIDTH=1200;


int main(int argc, char *argv[]) try {

    int m = 40;
    double a = 2.0;
    double r = 0.05;
    double K_ = 1;
    double T = 365 * 2;
    double dt = 3;

    createQtInterface(argc, argv, &a, &r, &K_, &T, &dt);
    if (!std::isfinite(T) || !std::isfinite(dt) || T <= 0 || dt <= 0 || T/dt > 1000000)
        throw std::invalid_argument("T and dt must be positive finite days; at most 1000000 steps");
    PricingParameters parameters;
    parameters.s1 = parameters.s2 = a/4; // A fixed mesh node throughout the curve.
    parameters.strike = K_;
    parameters.rate = r;
    parameters.maturity = T/365.0;
    const int time_steps = static_cast<int>(std::ceil(T/dt));
    std::vector<Vecteur<double>> history;
    const auto solution = priceFiniteElements(parameters, a, m, time_steps, &history);
    const auto mc = priceMonteCarlo(parameters);
    const double fe = solution.priceAt(parameters.s1, parameters.s2);
    std::cout << std::setprecision(10)
              << "Spots: " << parameters.s1 << ", " << parameters.s2
              << "\nFEM: " << fe << "\nMonte Carlo: " << mc.price
              << "\nMC 95% CI: [" << mc.ci_low << ", " << mc.ci_high << "]"
              << "\nFEM - MC: " << fe-mc.price << std::endl;
    solution.mesh.Save("Maillage.txt");
    matrice P(static_cast<int>(history.size()), static_cast<int>(solution.mesh.points.size()));
    std::vector<double> time, data;
    const int spot_index = (m/4)*(m+1)+m/4;
    for (std::size_t i=0; i<history.size(); ++i) {
        time.push_back(T*i/time_steps); // Display days; solver uses years.
        data.push_back(history[i][spot_index]);
        P.setRow(static_cast<int>(i), history[i]);
    }
    output("Solution.txt", P, time);
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "FEM price vs time to maturity (days); MC comparison in terminal");


    double maxi_data = std::max(max(data), 1e-8);
    double maxi_time=max(time);

    sf::CircleShape point(3); // Radius of 3 pixels
    point.setFillColor(sf::Color::Red); // Color of the point

    // Set the position of the point
    double initX=10;
    double initY=HEIGHT/2;
    //cout<<"max data = "<<maxi_data<<endl;
    //cout<<"max time = "<<maxi_time<<endl;
    //cout<<"time size = "<<time.size()<<endl;
    //cout<<"data size = "<<data.size()<<endl;
    double xScale=(WIDTH-40)/maxi_time;
    double yScale=(HEIGHT/2-10)/maxi_data;
    // loading font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return 1;
    }

    //draw x axis
    drawLine(window,initX,initY,WIDTH-15,initY,sf::Color::White,1);

    sf::ConvexShape triangle;
    triangle.setPointCount(3); // Define the number of points (vertices) in the shape

    // Define the points (vertices) of the triangle
    triangle.setPoint(0, sf::Vector2f(WIDTH-15, initY-5)); // Top point
    triangle.setPoint(1, sf::Vector2f(WIDTH-15, initY+5)); // Bottom-left point
    triangle.setPoint(2, sf::Vector2f(WIDTH-10, initY)); // Bottom-right point

    // Set the fill color of the triangle
    triangle.setFillColor(sf::Color::White);
    window.draw(triangle);

    sf::Text xLabel("Time", font, 12); // Label text and font size
    xLabel.setPosition(WIDTH - 30, initY + 10); // Position the label at the end of the x-axis
    window.draw(xLabel);
    //draw grads of x axis
    for (size_t i = 1; i < time.size(); ++i) {
        if(i%10==0){
            double x=initX+time[i]*xScale;
            drawLine(window,x,initY-5,x,initY+5,sf::Color::White,1);
            std::string textString = std::to_string((int)time[i]);
            sf::Text xLabel(textString, font, 10); // Label text and font size
            xLabel.setPosition(x-10, initY-7 + 10); // Position the label at the end of the x-axis
            window.draw(xLabel);

        }
    }
    //draw y axis
    drawLine(window,initX,5,initX,HEIGHT-5,sf::Color::White,1);
    
    //draw y grads
    int nbr = (int) ((HEIGHT/2-10)/yScale);
    for(int i=1;i<=nbr;i++){
        double positif_y = initY-i*yScale;
        double negatif_y = initY+i*yScale;
        drawLine(window,initX-5,positif_y,initX+5,positif_y,sf::Color::White,1);
        drawLine(window,initX-5,negatif_y,initX+5,negatif_y,sf::Color::White,1);
        std::string textString1 = std::to_string(1.0*i/nbr*maxi_data);
        sf::Text xLabel1(textString1, font, 10); // Label text and font size
        xLabel1.setPosition(initX+10, positif_y); // Position the label at the end of the x-axis
        window.draw(xLabel1);
        std::string textString2 = std::to_string(-1.0*i/nbr*maxi_data);
        sf::Text xLabel2(textString2, font, 10); // Label text and font size
        xLabel2.setPosition(initX+10, negatif_y); // Position the label at the end of the x-axis
        window.draw(xLabel2);


    }
    // Main loop
    for (size_t i = 0; i < time.size(); ++i)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window

        // Add the new point to the graph
        double x = initX + time[i] * xScale;  // Scale the x-coordinate
        double y = initY - data[i] * yScale; // Scale the y-coordinate
        //cout<<"data = "<<data[i]<<endl;
        //cout<<"x = "<<x<<endl;
        //cout<<"y = "<<y<<endl;
        if(i>0) {
            double previousX = initX + time[i-1] * xScale;
            double previousY = initY - data[i-1] * yScale;
            drawLine(window,previousX,previousY,x,y,sf::Color::Cyan,3);
            if (i%10==0){
                std::string textString = std::to_string((int)time[i]);
                sf::Text xLabel(textString, font,10); // Label text and font size
                xLabel.setPosition(x-10, initY-7 + 10); // Position the label at the end of the x-axis
                xLabel.setFillColor(sf::Color::Red);
                window.draw(xLabel);
            }
        }

        point.setPosition(x, y);
        // Draw the graph
        //window.draw(point);

        // Display the window
        window.display();

        // Wait for a short delay before updating the graph
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    // Keep the window open until closed by the user
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}

catch (const std::exception& e) {
    std::cerr << "Pricing error: " << e.what() << std::endl;
    return 1;
}
