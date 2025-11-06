QT += widgets
QMAKE_CLEAN += $$TARGET
SOURCES += main.cpp \
           utils.cpp \
           maillage.cpp \
           point.cpp \
           triangle.cpp \
           matrice.cpp \
           elementsFinies.cpp \
           monteCarlo.cpp \
           monteCarloVisualization.cpp

HEADERS += monteCarlo.hpp \
           monteCarloVisualization.hpp \
           elementsFinies.hpp \
           maillage.hpp \
           matrice.hpp \
           point.hpp \
           triangle.hpp \
           utils.hpp \
           vecteur.hpp

LIBS += -lsfml-graphics \
        -lsfml-window \
        -lsfml-system