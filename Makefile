# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -std=c++11 -Wall

# SFML libraries
SFML_LIBRARIES = -lsfml-graphics -lsfml-window -lsfml-system

SOURCE = main.cpp maillage.cpp point.cpp triangle.cpp matrice.cpp elementsFinies.cpp utils.cpp monte_carlo.cpp fem_pricer.cpp

# The build target
TARGET = myprogram

all: $(TARGET)
	@echo "\n Build completed successfully! Run ./$(TARGET) to execute your program !"

$(TARGET): $(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE) $(CFLAGS) $(SFML_LIBRARIES)

clean:
	rm -f $(TARGET) compare_pricers pricing_tests

# Headless pricing and tests: no Qt/SFML dependency.
PRICING_SOURCES = monte_carlo.cpp fem_pricer.cpp maillage.cpp point.cpp triangle.cpp matrice.cpp elementsFinies.cpp
.PHONY: compare check
compare: compare_pricers
compare_pricers: compare_pricers.cpp $(PRICING_SOURCES) $(wildcard *.hpp)
	$(CC) -std=c++17 -O2 -Wall -Wextra -pedantic -o $@ compare_pricers.cpp $(PRICING_SOURCES)
pricing_tests: pricing_tests.cpp $(PRICING_SOURCES) $(wildcard *.hpp)
	$(CC) -std=c++17 -O2 -Wall -Wextra -pedantic -o $@ pricing_tests.cpp $(PRICING_SOURCES)
check: pricing_tests
	./pricing_tests
