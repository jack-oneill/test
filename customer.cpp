#include "customer.h"

Customer::Customer(uint64_t id, QString name, const lemon::SmartDigraph::Node& pos, const lemon::SmartDigraph::Node& dest, World *world) : Agent(id,name,CUSTOMER,pos,world)
{
    myDestination=dest;
    myVehicle=NULL;
    mySatisfaction=100;
    myWaiting=true;
    myRequestTime=0;
    myIdealDistance=1e8;
    myPickUpTime=0;
    myOptimalTime=0;
    myNeighborhoodId=-1;
    myHasVehicle=false;
}
lemon::SmartDigraph::Node Customer::origin()
{
    return position().first;
}
lemon::SmartDigraph::Node Customer::destination()
{
    return myDestination;
}

unsigned Customer::satisfaction()
{
    return mySatisfaction;
}
void Customer::setIdealDistance(const double& idist)
{
    myIdealDistance=idist;
}
void Customer::setNeighborhoodId(const int& nid)
{
    myNeighborhoodId=nid;
}

int Customer::neighborhoodId() const
{
   return myNeighborhoodId;
}

double Customer::idealDistance() const
{
    return myIdealDistance;
}

void Customer::setSatisfaction(const unsigned& sat)
{
    mySatisfaction=sat;
}

void Customer::setVehicle(Vehicle* veh)
{
    myVehicle=veh;
}

Vehicle* Customer::vehicle()
{
    return myVehicle;
}
void Customer::setWaitingToBePickedUp(bool wait)
{
    myWaiting=wait;
}

void Customer::setOptimalTime(const uint64_t& tim)
{
    myOptimalTime=tim;
}

uint64_t Customer::optimalTime() const
{
   return myOptimalTime;
}
void Customer::setPickUpTime(const uint64_t& tim)
{
   myPickUpTime=tim;
}

uint64_t Customer::pickUpTime() const
{
   return myPickUpTime;
}
bool Customer::waiting() const
{
    return myWaiting;
}
uint64_t Customer::requestTime() const
{
    return myRequestTime;
}
uint64_t Customer::setRequestTime(const uint64_t& tim)
{
    myRequestTime=tim;
}

void Customer::setHasVehicle(bool val)
{
    myHasVehicle=val;
}

bool Customer::hasVehicle() const
{
    return myHasVehicle;
}
