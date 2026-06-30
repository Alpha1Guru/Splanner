QT += core gui widgets

CONFIG += c++17

TARGET = StudyPlanner
TEMPLATE = app

SOURCES += main_qt.cpp \
           mainwindow.cpp \
           TaskManager.cpp

HEADERS += mainwindow.h \
           TaskManager.h \
           Task.h
