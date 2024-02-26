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

const double HEIGHT=900;
const double WIDTH=1200;


int main(int argc, char *argv[]){

    int m = 10, n = 10;
    double a = 2.0;
    double r = 0.05;
    double K_ = 1;
    double T = 365 * 2;
    double dt = 3;

    createQtInterface(argc, argv, &a, &r, &K_, &T, &dt);
    //constexpr double nbr_pas = T / dt;

    // Creation du maillage
    //std::cout << "Creation du maillage:";
    Maillage mc(0, a, 0, a, m, n);
    mc.Save("Maillage.txt");
    long int Nbpts = mc.points.size(), Nbtri = mc.triangles.size();
    std::vector<Point> Coorneu = mc.points;
    std::vector<Triangle> Numtri = mc.triangles;

    matrice M(Nbpts, Nbpts);
    matrice K(Nbpts, Nbpts);
    matrice B(Nbpts, Nbpts);

    for (int l = 0; l < Nbtri; l++)
    {
        //double r = (1.0 * l / Nbtri) * 100;
        //std::cout << "\rCalcul des matrices elements fini:" << round(r) <<"%"<< std::flush;
        Point S1 = Coorneu[Numtri[l].s1];
        Point S2 = Coorneu[Numtri[l].s2];
        Point S3 = Coorneu[Numtri[l].s3];

        // Calcul des matrices elementaires M_el, K_el, B_el
        matrice M_el, K_el, B_el;
        M_el = matM_el(S1, S2, S3);
        K_el = matK_el(S1, S2, S3);
        B_el = matB_el(S1, S2, S3);

        // Remplissage des matrices M, K, B
        for (int i = 0; i < M_el.getNbRows(); i++)
        {
            int I = Numtri[l].getS(i + 1);
            for (int j = 0; j < M_el.getNbCols(); j++)
            {
                int J = Numtri[l].getS(j + 1);
                M.set(I, J, M.val(I, J) + M_el.val(i, j));
                K.set(I, J, K.val(I, J) + K_el.val(i, j));
                B.set(I, J, B.val(I, J) + B_el.val(i, j));
            }
        }
    }
    //cout<<"\n";

    // Shema numerique
    std::vector<double> Time = timeSlots(0, T, dt);
    int steps = Time.size();
    matrice P(steps, Nbpts);
    Vecteur<double> Q_0(Nbpts, 0);
    for (int l = 0; l < Nbpts; l++)
    {
        Point P = Coorneu[l];
        double x1 = P.x, x2 = P.y;
        Q_0[l] = (x1 + x2 - K_ > 0) ? x1 + x2 - K_ : 0;
    }

    matrice D = r * M + K + B;

    P.setRow(0, Q_0);
    // Create a window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML Graph");
    int t=1;
    while (window.isOpen() and t<steps){
        //double r = (double(t + 1) / steps) * 100;
        //std::cout << "\rCalcul du schema numerique:" << round(r) << "%" <<  std::flush;
        drawProgressBar(window,t, steps);
        P.setRow(t, resoudre((M + (dt / 2) * D), (M - (dt / 2) * D) * P.getRow(t - 1)));
        t++;
    }
    //cout<<"\n";
    window.clear();
    mc.Save("Maillage.txt");
    output("Solution.txt", P, Time);

    // Example data for time axis and data values
    std::vector<double> time = Time;
    std::vector<double> data;
    unsigned long nb=0;
    for (int i = 0; i < P.getNbRows(); i++)
    {
        for (int j = 0; j < P.getNbCols(); j++)
        {
            if (nb==time.size()) break;
            if(P(i,j)>0 ) data.push_back(log(P(i, j)));
            else if(P(i,j)<0) data.push_back(-log(-P(i,j)));
            else data.push_back(0); 
            nb++;
        }
    }


    double maxi_data = max(data);
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
