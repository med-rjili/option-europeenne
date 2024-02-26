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