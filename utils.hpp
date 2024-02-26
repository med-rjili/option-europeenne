#ifndef UTILS_HPP
#define UTILS_HPP
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QPixmap>
#include <QPalette>
#include <QLabel>
#include <QHBoxLayout> // Include QHBoxLayout for horizontal layout
#include <QSpacerItem> // Include QSpacerItem for adding spacer

int createQtInterface(int argc, char *argv[], double *a, double *r, double *k, double *t, double *dt);
void drawLine(sf::RenderWindow& window, int x1, int y1, int x2, int y2, sf::Color color, float thickness);
double max(std::vector<double> &v);
void drawProgressBar(sf::RenderWindow& window, float currentProgress, int finalIndex);
#endif