# European Option Simulation Project

This project simulates European options using C++ and object-oriented programming (OOP) with a focus on finite element mathematics.

## Overview

The project aims to implement a simulation of European options, which are financial derivatives that give the holder the right, but not the obligation, to buy or sell an underlying asset at a predetermined price (the strike price) on or before the expiration date. The simulation will use C++ for the implementation and apply concepts of object-oriented programming (OOP) to model the options and their behavior. Additionally, the project will leverage finite element mathematics to calculate and analyze option prices and related metrics.

## Features

- **European Option Class**: Implementation of a class to represent European options, including attributes such as underlying asset price, strike price, expiration date, volatility, and interest rate.
- **Finite Element Methods**: Application of finite element methods to discretize and solve option pricing differential equations.

## running

to run project:

```bash
g++ -o test main.cpp option.cpp point.cpp && ./test
```
