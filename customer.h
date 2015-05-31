#ifndef CUSTOMER_H
#define CUSTOMER_H
#include "agent.h"
class Vehicle;
class Customer : public Agent
{
protected:
    lemon::SmartDigraph::Node myDestination;
    unsigned mySatisfaction;
    Vehicle* myVehicle;
    bool myWaiting;
    bool myHasVehicle;
    uint64_t myRequestTime;
    uint64_t myOptimalTime;
    uint64_t myPickUpTime;
    int myNeighborhoodId;

    double myIdealDistance;
public:
    Customer(uint64_t id, QString name, const lemon::SmartDigraph::Node& pos,const lemon::SmartDigraph::Node& des, World* world);
    lemon::SmartDigraph::Node origin();
    lemon::SmartDigraph::Node destination();
    unsigned satisfaction();
    void setIdealDistance(const double&);
    void setNeighborhoodId(const int&);
    int neighborhoodId() const;
    double idealDistance() const;
    void setSatisfaction(const unsigned&);
    void setVehicle(Vehicle*);
    Vehicle* vehicle();
    void setWaitingToBePickedUp(bool=true);
    void setOptimalTime(const uint64_t&);
    uint64_t optimalTime() const;
    void setPickUpTime(const uint64_t&);
    uint64_t pickUpTime() const;
    void setHasVehicle(bool);
    bool hasVehicle() const;
    bool waiting() const;
    uint64_t requestTime() const;
    uint64_t setRequestTime(const uint64_t&);

};

#endif // CUSTOMER_H
