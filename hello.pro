TEMPLATE = app
TARGET = RestaurantOrderingSystem
QT += core gui

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

# Add include and lib paths for Qt
INCLUDEPATH += C:/msys64/mingw64/include
LIBS += -LC:/msys64/mingw64/lib
