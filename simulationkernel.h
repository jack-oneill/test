#ifndef SIMULATIONKERNEL_H
#define SIMULATIONKERNEL_H

#include <QThread>
#include <QMap>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QTime>
#include "world.h"
#include "event.h"
#include "eventqueue.h"

class SimulationKernel : public QObject
{
    Q_OBJECT
private:
    //QMutexLocker* myLock;
    QTime myTime;
    QMutex* myViewRefreshMutex;
    QWaitCondition* myCondition;
protected:
    World* myWorld;
    uint64_t myCurrentTime;
    uint64_t myTimeLimit;
    EventQueue* myEventQueue;
    QList<Event*> myTriggeredEvents;
    bool mySimulationStopped;
    bool mySimulationPaused;
    double myTimeWarp;
    inline void sleep(uint64_t);
    void processDeltaCycles();
    void eventLoop();
    void initializeSimulation();
    void endSimulation();
    void pushEvent(Event*);
    void popEvent();
    friend class Event;
    friend class Agent;
public:
    explicit SimulationKernel(QObject *parent = 0);
    ~SimulationKernel();
    QList<Agent*> agents();
    QList<Vehicle*> vehicles();
    QList<Customer*> customers();
    void setTimeWarp(double);
    void setWorld(World*);
    World* world();
    uint64_t time() const;
    void visualizeMe(Agent*);
    void removeEvents(Agent*);
    void setTimeLimit(uint64_t hours);
    QMutex& viewRefreshMutex();
    bool existsEvent(const uint64_t&,const EventType&,Agent*) const;
signals:
    void visualize();
    void simulationEnded();
    void simulationStarted(const QString&);
    void resetWorld();
public slots:
    void start();
    void pause();
    void stop();

};

#endif // SIMULATIONKERNEL_H
