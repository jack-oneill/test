#ifndef VEHICLE_H
#define VEHICLE_H
#include <QList>
#include <QPair>
#include <QSet>
#include "agent.h"
#include "customer.h"
#include <QMutex>
class Vehicle : public Agent
{
protected:
    QMutex* myMutex;
    unsigned myCapacity;
    QList<QPair<lemon::SmartDigraph::Node,double> > myRouteAndSpeed;
    QSet<Customer*> myCustomers;
    int myCurrentPos;
    uint64_t myLastMoveTime;
    double myInitialOffset;
    QSet<Customer*> myCustomersWithinReach;
public:
    Vehicle(uint64_t,unsigned,QString,const lemon::SmartDigraph::Node&,World*);
    virtual ~Vehicle();
    unsigned capacity();
    QSet<Customer*> customers();
    void setRouteAndSpeed(const QList<QPair<lemon::SmartDigraph::Node,double> >&, const double & =0);
    void addCustomer(Customer*);
    void removeCustomer(Customer*);
    QList<QPair<lemon::SmartDigraph::Node, double> > routeAndSpeed();
    lemon::SmartDigraph::Node nextPosition();
    lemon::SmartDigraph::Arc arc();
    virtual QPair<lemon::SmartDigraph::Node,double> position(const uint64_t&);
    double speed();
    double initialOffset() const;
    void move(const uint64_t&);
    QSet<Customer*> customersWithinReach() const;
    void setCustomersWithinReach(const QSet<Customer*>&);
    uint64_t lastMoveTime() const;
};

#endif // VEHICLE_H
