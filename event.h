#ifndef EVENT_H
#define EVENT_H
#include <stdlib.h>
#include <QPointF>

class Agent;
class Customer;
class Vehicle;
class SimulationKernel;
class CustomerGenerator;
enum EventType  {t_EventStartSimulation,t_EventMove,t_EventPickUp,t_EventDropOff,t_EventShowUp,t_EventDisappear,t_EventSatisfactionDown,t_EventSnooze};
#define SNOOZETIME 600

class Event
{

protected:
    Agent* myAgent;
    EventType myType;
    SimulationKernel* myKernel;
    uint64_t myTime;
public:
    Event(Agent*,uint64_t time,SimulationKernel*);
    EventType type();
    uint64_t time();
    Agent* agent();
    virtual void execute()=0;

};

class EventStartSimulation  : public Event
{
public:
    EventStartSimulation(SimulationKernel*);
    virtual void execute();
};

class EventMove : public Event
{
public:
    EventMove(Vehicle*,uint64_t,SimulationKernel*);
    virtual void execute();
};

class EventPickUp : public Event
{
private:
    Customer* myCustomer;
public:
    EventPickUp(Customer*, Vehicle*,uint64_t,SimulationKernel*);
    Customer* customer();
    virtual void execute();
};

class EventDropOff : public Event
{
private:
    Customer* myCustomer;
public:
    EventDropOff(Customer*, Vehicle*,uint64_t,SimulationKernel*);
    Customer* customer();
    virtual void execute();
};

class EventShowUp : public Event
{
protected:
    void assignVehicle();
private:
    CustomerGenerator* myGenerator;
public:
    EventShowUp(Customer*,uint64_t,SimulationKernel*);
    virtual void execute();
};

class EventDisappear : public Event
{
public:
    EventDisappear(Customer*,uint64_t,SimulationKernel*);
    virtual void execute();
};

class EventSatisfactionDown : public Event
{
protected:
    unsigned myStep;
public:
    EventSatisfactionDown(unsigned,Customer*,uint64_t,SimulationKernel*);
    unsigned step();
    virtual void execute();
};

class EventSnooze : public EventShowUp
{
protected:
    uint64_t myDelta;
public:
    EventSnooze(Customer*,uint64_t,uint64_t,SimulationKernel*);
    virtual void execute();
};


#endif // EVENT_H
