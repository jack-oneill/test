#-------------------------------------------------
#
# Project created by QtCreator 2014-10-26T22:58:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(opengl): QT += opengl

TARGET = RouteSim
TEMPLATE = app

macx
{
    INCLUDEPATH+=/usr/local/include
    INCLUDEPATH+=/Users/vladimir/local/include
    LIBS+= -L/usr/local/lib
    LIBS+=-L/Users/vladimir/local/lib -lemon
}

SOURCES += main.cpp\
        mainwindow.cpp \
    agent.cpp \
    simulationkernel.cpp \
    event.cpp \
    vehicle.cpp \
    customer.cpp \
    agentfactory.cpp \
    eventqueue.cpp \
    world.cpp \
    vehiclerouter.cpp \
    customergenerator.cpp \
    gui/simgraphicsview.cpp \
    gui/vehicleview.cpp \
    gui/customerview.cpp \
    customergenerationprocess.cpp \
    routingnetwork.cpp \
    gui/streetline.cpp \
    gui/logger.cpp \
    gui/settings.cpp \
    gui/statistics.cpp \
    customergeneratoroffline.cpp \
    regressionrunner.cpp \
    outputgenerator.cpp

HEADERS  += mainwindow.h \
    agent.h \
    simulationkernel.h \
    event.h \
    vehicle.h \
    customer.h \
    agentfactory.h \
    eventqueue.h \
    world.h \
    vehiclerouter.h \
    customergenerator.h \
    gui/simgraphicsview.h \
    gui/vehicleview.h \
    gui/customerview.h \
    customergenerationprocess.h \
    routingnetwork.h \
    gui/streetline.h \
    gui/logger.h \
    gui/settings.h \
    gui/statistics.h \
    customergeneratoroffline.h \
    regressionrunner.h \
    outputgenerator.h
