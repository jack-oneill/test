#ifndef VEHICLEROUTER_H
#define VEHICLEROUTER_H
#include <QPointF>
#include <vehicle.h>
#include <lemon/maps.h>
#include <lemon/smart_graph.h>
#include <lemon/concepts/graph.h>
#include <lemon/list_graph.h>
#include <QList>
class World;
class Customer;
class RoutingNetwork;

#define SATISFACTION_TRESHOLD 0.7


#define SPEED 6.5 //in m/s
#define WAITING_TIME 15.0// in minutes
class AStarCompare
{
protected:
    lemon::IterableValueMap<lemon::SmartDigraph,lemon::SmartDigraph::Node,double>* myEstimate;
public:
    AStarCompare(lemon::IterableValueMap<lemon::SmartDigraph,lemon::SmartDigraph::Node,double>&);
    bool operator() (const lemon::SmartDigraph::Node&, const lemon::SmartDigraph::Node&) const;
};

class VehicleRouter
{
protected:
    World* myWorld;
    bool myIgnoreWaitingTime;
    QPointF toPoint (const lemon::SmartDigraph::Node& n );
    lemon::SmartDigraph::Node toNode (const QPointF& n );
    static bool comparator(std::pair<Vehicle*,qreal> i, std::pair<Vehicle*,qreal> j);
    static bool comparator2 (std::pair <QList <lemon::SmartDigraph::Node>,double> i, std::pair <QList <lemon::SmartDigraph::Node>,double> j);
    static bool comparator3 (QPair<lemon::SmartDigraph::Node, double> i,QPair<lemon::SmartDigraph::Node, double> j);
    double getPathLength (const QList <lemon::SmartDigraph::Node>& route_for_customer,RoutingNetwork* net);
    bool canAddCustomer(const QList <lemon::SmartDigraph::Node>& route,Vehicle* agent, Customer* cust);
    QList<std::pair <Vehicle*, qreal> > getDistancesFromCustomer (Customer* cust);
    QPair <QList<lemon::SmartDigraph::Node>,QList < QPair <lemon::SmartDigraph::Node,lemon::SmartDigraph::Node> > > getStopNodes (Vehicle* agent, Customer* cust);
    bool legitPermutation (const QList<lemon::SmartDigraph::Node>& current, const QList< QPair <lemon::SmartDigraph::Node,lemon::SmartDigraph::Node> >& odlist);
    QList < QList <lemon::SmartDigraph::Node> > getPermutations (QList<lemon::SmartDigraph::Node> nodes);
    QList < QList <lemon::SmartDigraph::Node> > getPermutations2 (QList<lemon::SmartDigraph::Node> nodes, Vehicle* vehicle,  QList < QPair < lemon::SmartDigraph::Node, lemon::SmartDigraph::Node> > odlist);
private:
    static bool myNodeCompare(lemon::SmartDigraph::Node a,lemon::SmartDigraph::Node b);
    static VehicleRouter* myInstance;
    VehicleRouter(World*);
public:
    static VehicleRouter* instance(World* = NULL);
    void setWorld(World*);
    void setIgnoreWaitingTime (bool);
    World* world() const;
    virtual Vehicle* route(Customer*);
    std::pair <QList <lemon::SmartDigraph::Node>,double> getRoute (const lemon::SmartDigraph::Node& s,const lemon::SmartDigraph::Node& t,RoutingNetwork* net);
    std::pair <QList <lemon::SmartDigraph::Node>,double> getRouteAStar (const lemon::SmartDigraph::Node& s,const lemon::SmartDigraph::Node& t,RoutingNetwork* net);
};

#endif // VEHICLEROUTER_H
