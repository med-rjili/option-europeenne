QT += widgets
QMAKE_CLEAN += $$TARGET
SOURCES += main.cpp \
           utils.cpp \
           maillage.cpp \
           point.cpp \
           triangle.cpp \
           matrice.cpp \
           elementsFinies.cpp \


LIBS += -lsfml-graphics \
        -lsfml-window \
        -lsfml-system
SOURCES += monte_carlo.cpp fem_pricer.cpp
CONFIG += c++17
