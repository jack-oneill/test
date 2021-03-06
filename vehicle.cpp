#include "vehicle.h"
#include <QDebug>
#include <cmath>
#include "world.h"
#include <QMutexLocker>
Vehicle::Vehicle( uint64_t id,unsigned capacity, QString name, const lemon::SmartDigraph::Node&  pos, World *world) : Agent(id, name,VEHICLE,pos,world)
{
    myCapacity=capacity;
    myInitialOffset=0;
    myCurrentPos=0;
    myMutex = new QMutex();
}
Vehicle::~Vehicle()
{
    delete myMutex;
}
unsigned Vehicle::capacity()
{
    return myCapacity;
}

void Vehicle::addCustomer(Customer* customer)
{
    myCustomers.insert(customer);
}

void Vehicle::removeCustomer(Customer* customer)
{
    myCustomers.remove(customer);
}

QSet<Customer*> Vehicle::customers()
{
    return myCustomers;
}

void Vehicle::setRouteAndSpeed(const QList<QPair<lemon::SmartDigraph::Node, double> > &rs, const double& offset)
{
    QMutexLocker locker(myMutex);
    myRouteAndSpeed=rs;
    myInitialOffset=offset;
    myCurrentPos=0;
    if(myRouteAndSpeed.size()>0)
        myPosition=myRouteAndSpeed.front().first;
    /*else
        myPosition=lemon::INVALID;*/
}

QList<QPair<lemon::SmartDigraph::Node, double> > Vehicle::routeAndSpeed()
{
    return myRouteAndSpeed;
}


lemon::SmartDigraph::Arc Vehicle::arc()
{
    QMutexLocker locker(myMutex);
    lemon::SmartDigraph::Arc prevPos=lemon::INVALID;
    if(myRouteAndSpeed.size()<2)
        return prevPos;
    if(myCurrentPos==0)
    {
        for(lemon::SmartDigraph::OutArcIt oait(*myWorld->network()->graph(),myRouteAndSpeed[myCurrentPos].first);oait!=lemon::INVALID;++oait)
        {
            if(myWorld->network()->graph()->target(oait)==nextPosition())
                return oait;
        }
    }

    for(lemon::SmartDigraph::InArcIt iait(*myWorld->network()->graph(), myRouteAndSpeed[myCurrentPos].first);iait!=lemon::INVALID;++iait)
    {
        if(myWorld->network()->graph()->source(iait)==myRouteAndSpeed[myCurrentPos-1].first)
            return iait;
    }
    return lemon::INVALID;
}

lemon::SmartDigraph::Node Vehicle::nextPosition()
{
    if(myCurrentPos+1>=myRouteAndSpeed.size())
    {
        if(myRouteAndSpeed.size()==0)
            return myPosition;
        return myRouteAndSpeed.back().first;
    }
    return myRouteAndSpeed[myCurrentPos+1].first;
}

QPair<lemon::SmartDigraph::Node,double> Vehicle::position(const uint64_t& time)
{
    QMutexLocker locker(myMutex);
    QPair<lemon::SmartDigraph::Node,double> rval;
    if(myCurrentPos==0 && myRouteAndSpeed.size()==0)
    {
        return Agent::position();
    }
    if(myCurrentPos==myRouteAndSpeed.size()-1)
    {
        rval.first=myRouteAndSpeed.back().first;
        rval.second=0;
        return rval;
    }
   lemon::SmartDigraph::Node prev = myRouteAndSpeed[myCurrentPos].first;
   lemon::SmartDigraph::Node next=myRouteAndSpeed[myCurrentPos+1].first;
   lemon::SmartDigraph::Arc arc;
   lemon::SmartDigraph* graph = myWorld->network()->graph();
   for(lemon::SmartDigraph::OutArcIt oait(*graph,prev);oait!=lemon::INVALID;++oait)
   {
       if(graph->target(oait)==next)
           arc = oait;
   }
   double length = (*myWorld->network()->distanceMap())[arc];
   double vel = myRouteAndSpeed[myCurrentPos].second;
   //vel = vel/36;
   double ratio = 0;
   if(myCurrentPos==0)
       ratio = vel*(time-myLastMoveTime)/((1-myInitialOffset)*length);
   else
       ratio = vel*(time-myLastMoveTime)/length;
   rval.first=prev;
   rval.second=ratio;
   return rval;
}

double Vehicle::speed()
{
    QMutexLocker locker(myMutex);
    if(myCurrentPos>=myRouteAndSpeed.size())
        return 0;
    return myRouteAndSpeed[myCurrentPos].second;
}
double Vehicle::initialOffset() const
{
   return myInitialOffset;
}

void Vehicle::move(const uint64_t& time)
{
    myLastMoveTime=time;
    myCurrentPos++;
    myPosition=myRouteAndSpeed[myCurrentPos].first;
    if(myPosition==lemon::INVALID)
        qDebug()<<"ERROR: Position Invalid for Vehicle "+QString::number(this->id());
}
QSet<Customer*> Vehicle::customersWithinReach() const
{
   return myCustomersWithinReach;
}
void Vehicle::setCustomersWithinReach(const QSet<Customer*>& creach)
{
   myCustomersWithinReach=creach;
}
