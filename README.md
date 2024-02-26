# European Option Simulation Project

This project simulates European options using C++ and object-oriented programming (OOP) with a focus on finite element mathematics.

## Overview

The project aims to implement a simulation of European options, which are financial derivatives that give the holder the right, but not the obligation, to buy or sell an underlying asset at a predetermined price (the strike price) on or before the expiration date. The simulation will use C++ for the implementation and apply concepts of object-oriented programming (OOP) to model the options and their behavior. Additionally, the project will leverage finite element mathematics to calculate and analyze option prices and related metrics.

## Features

- **European Option Class**: Implementation of a class to represent European options, including attributes such as underlying asset price, strike price, expiration date, volatility, and interest rate.
- **Finite Element Methods**: Application of finite element methods to discretize and solve option pricing differential equations.

## dependencies

### QT

Qt is cross-platform software for creating graphical user interfaces
run `sudo apt install qt5-default` to install QT

### SFML

Simple and Fast Multimedia Library is a cross-platform software development library designed to provide a simple application programming interface to various multimedia components in computers.
we used SFML to plot the graph of our results.

run `sudo apt-get install libsfml-dev` to install SFML

## running

to run project:

```bash
qmake
make
./option-europeenne
```

## to clear

```bash
make clean
```
