#include "vehiclerouter.h"
#include "customer.h"
#include "world.h"
#include"vehicle.h"
#include <cmath>
#include <lemon/dijkstra.h>
#include "routingnetwork.h"
#include<lemon/concepts/graph.h>
#include <lemon/list_graph.h>
#include <QDebug>
using namespace lemon;

VehicleRouter* VehicleRouter::myInstance=NULL;

bool VehicleRouter::myNodeCompare(lemon::SmartDigraph::Node a,lemon::SmartDigraph::Node b)
{
    return  a<b;
}

VehicleRouter::VehicleRouter(World* world)
{
    myWorld=world;
    myIgnoreWaitingTime=false;
}

//TO DO: Implement this function that takes a node and find the Qpoint
QPointF VehicleRouter::toPoint (const SmartDigraph::Node& n )
{
    if(myWorld==NULL)
        return QPointF();
    return (*myWorld->network()->positionMap())[n];
}

//TO DO: Implement this function that takes a Qpoint and finds the node
SmartDigraph::Node VehicleRouter::toNode (const QPointF& n )
{
    SmartDigraph::Node a;
    return a;
}

void VehicleRouter::setIgnoreWaitingTime(bool ignore)
{
    myIgnoreWaitingTime=ignore;
}

//Computes the route between s and t and returns a pair consisting of the list of nodes and the length of the path
std::pair <QList <SmartDigraph::Node>,double> VehicleRouter::getRoute (const SmartDigraph::Node& s, const SmartDigraph::Node& t,RoutingNetwork* net)
{
    std::pair <QList <SmartDigraph::Node>,double> result;
    Dijkstra<SmartDigraph, RoutingNetwork::DistanceMap> dijkstra_test(*net->graph(),*net->distanceMap());
    dijkstra_test.run(s);
    double path_length=dijkstra_test.dist(t);
    QList <SmartDigraph::Node> path;
    for (SmartDigraph::Node v=t;v != s; v=dijkstra_test.predNode(v)) {
      path.push_front(v);
    }
    path.push_front(s);
    if (path.size()==1){
        qDebug()<<"path has one node!Origin ";
        if (s==t){
            qDebug()<<"Origin and destination are the same!";
        }
    }

    result.first=path;
    result.second=path_length;
    return result;
}

bool VehicleRouter::comparator(std::pair<Vehicle*,qreal> i, std::pair<Vehicle*,qreal> j)
{return ((i.second < j.second));}

bool VehicleRouter::comparator2 (std::pair <QList <SmartDigraph::Node>,double> i,std::pair <QList <SmartDigraph::Node>,double> j)
{return ((i.second < j.second));}

bool VehicleRouter::comparator3 (QPair<SmartDigraph::Node, double> i,QPair<SmartDigraph::Node, double> j)
{return ((i.second < j.second));}

//Given a path that is a list of nodes this function copmutes its length
double VehicleRouter::getPathLength (const QList <SmartDigraph::Node>& route_for_customer,RoutingNetwork* net){
    double current=0.0;
    foreach (SmartDigraph::Node n, route_for_customer) {
       if (route_for_customer.indexOf(n)+1<route_for_customer.size()){
            SmartDigraph::Node from;
            SmartDigraph::Node to;
            from=n;
            to=route_for_customer.at(route_for_customer.indexOf(n)+1);
            SmartDigraph::Arc a;
            SmartDigraph* g=net->graph();
            for (SmartDigraph::OutArcIt oait(*g,from); oait != INVALID; ++oait) {
                if(g->target(oait)==to){
                    a=oait;
                }
            }

            double arclength=net->distanceMap()->operator [](a);
            current=current+arclength;
       }
    }
    return current;
}
//Checks if the customer can be added to this vehicle. Checking waiting times for passengers not yet in the car and overal delay compared to ideal case
bool VehicleRouter::canAddCustomer(const QList <SmartDigraph::Node>& route,Vehicle* agent, Customer* cust){
    bool can=false;

    QSet <Customer*> customers=agent->customers();
    customers.insert(cust);
    if (customers.size()<agent->capacity()){
        can=true;
        foreach (Customer* c, customers) {
            SmartDigraph::Node origin=c->origin();
            SmartDigraph::Node destination=c->destination();
            if (c->waiting()){
                QList <SmartDigraph::Node> route_to_customer=route.mid(0,route.indexOf(origin)+1);
                double distance_to_customer=getPathLength(route_to_customer,c->world()->network());
                double waiting_time_customer=c->world()->time()-c->requestTime()+distance_to_customer*1000/SPEED;//in seconds
                if (!myIgnoreWaitingTime)
                {
                    if (waiting_time_customer/60.0>WAITING_TIME){can=false;}
                }
            }
            if (can){
                QList <SmartDigraph::Node> route_for_customer;
                route_for_customer=route.mid(route.indexOf(origin),route.indexOf(destination)-route.indexOf(origin)+1);//Check if it gets the whole route
                double ideal=cust->idealDistance();//getRoute(route_for_customer.first(),route_for_customer.last(),agent->world()->network()).second;//TO DO: This should be copmuted only once when the customer is generated and kept as an attrtibute
                double current=getPathLength(route_for_customer,cust->world()->network());

                if (ideal/current<SATISFACTION_TRESHOLD){can=false;}
            }//fix kilometers and meters problem. it probably is in km
        }
    }
    return can;
}
//Computes the distances from every vechile that has at least one space and is less the the waiting time away to the customer
QList<std::pair <Vehicle*, qreal> > VehicleRouter::getDistancesFromCustomer (Customer* cust)
{
    SmartDigraph::Node customer_origin;
    SmartDigraph::Node customer_destination;
    customer_origin=(cust->origin());
    customer_destination=(cust->destination());
    World* w=cust->world();
    QList<Vehicle*> agent_list=w->vehicles();
    QList<Vehicle*>::iterator it;
    QList<std::pair <Vehicle*, qreal> > distances;
    for(it=agent_list.begin(); it!=agent_list.end(); it++)
    {
        std::pair<Vehicle*,qreal> current;
        current.first=(*it);
        QPointF origin_position=toPoint(customer_origin);
        QPointF vehicle_position=toPoint((*it)->nextPosition());
        current.second=(origin_position-vehicle_position).manhattanLength()*40000/360;
        //Check if this car has free space and also calculate if it is possible for the car to reach the customer within the waiting time
        if (myIgnoreWaitingTime)
        {
            if (current.first->customers().size()<current.first->capacity()){
                distances.append(current);
            }
        }
        else
        {
            if (current.second/SPEED/60*1000<WAITING_TIME && current.first->customers().size()<current.first->capacity()){
                distances.append(current);
            }
        }

    }
    sort(distances.begin(),distances.end(),comparator);
    return distances;
}

//Returns the nodes through which the car has to pass and also the sets of origins and destinations of customers not yet in the car in order to make sure later that the customer is picked up before he is delivered
QPair <QList <SmartDigraph::Node>,QList < QPair < SmartDigraph::Node, SmartDigraph::Node> > > VehicleRouter::getStopNodes (Vehicle* agent, Customer* cust)
{
    QList<Customer*> customers_in_this_agent= agent->customers().toList();
    customers_in_this_agent.append(cust);
    QList <SmartDigraph::Node> nodes;
    QList < QPair < SmartDigraph::Node, SmartDigraph::Node> > odlist;//used for the agents that also need to visit their origins as well. used to check if the permutation first visits the origin and then the destination
    Customer* icust=NULL;
    for(int i=0;i<customers_in_this_agent.size();++i)
    {
        icust = customers_in_this_agent[i];
        nodes.append(icust->destination());
        if (icust->waiting())// ONLY WAY TO GET JUST THE DESTINATION IS IF THE CUSTOMER IS NOT WAITING ANYMORE
        {
            nodes.append(icust->origin());
            QPair < SmartDigraph::Node, SmartDigraph::Node> od;
            od.first=icust->origin();
            od.second=icust->destination();
            odlist.append(od);
        }
    }
    //nodes.append((cust->origin()));
    //nodes.append((cust->destination()));
    QPair <QList <SmartDigraph::Node>,QList < QPair < SmartDigraph::Node, SmartDigraph::Node> > > result;
    result.first=nodes;
    result.second=odlist;
    return result;
}

//Checks if the permutation visits the origins before the destinations
bool VehicleRouter::legitPermutation (const QList<SmartDigraph::Node>& current, const QList< QPair < SmartDigraph::Node, SmartDigraph::Node> >& odlist)
{
    bool result=true;
    QPair < SmartDigraph::Node, SmartDigraph::Node> od;
    foreach (od, odlist) {
       if (current.indexOf(od.first)>current.indexOf(od.second)){result=false;}
    }
    return result;
}

//Computes all the permutations of the important nodes
QList < QList <SmartDigraph::Node> > VehicleRouter::getPermutations (QList<SmartDigraph::Node> nodes)
{
     QList < QList <SmartDigraph::Node> > perms;
     QList <SmartDigraph::Node> nodes2;
//remove duplicating elements if such...
     foreach (SmartDigraph::Node n, nodes) {
         if (!nodes2.contains(n)){
             nodes2.push_back(n);
         }

     }
     std::sort(nodes2.begin(), nodes2.end());

     do {
         perms.push_back(nodes2);
     } while(std::next_permutation(nodes2.begin(), nodes2.end()));
     return perms;
 }

QList < QList <SmartDigraph::Node> > VehicleRouter::getPermutations2 (QList<SmartDigraph::Node> nodes, Vehicle* vehicle,  QList < QPair < SmartDigraph::Node, SmartDigraph::Node> > odlist)
{
     if (nodes.size()==0 || nodes.first()==nodes.last())
     {
         qDebug()<<"empty nodes";

     }

     QList < QList <SmartDigraph::Node> > perms;
     QList <SmartDigraph::Node> path;
     QList <SmartDigraph::Node> nodes2;
//remove duplicating elements if such...
     foreach (SmartDigraph::Node n, nodes) {
         if (!nodes2.contains(n)){
             nodes2.push_back(n);
         }

     }
    std::sort(nodes2.begin(), nodes2.end());

    QPair < SmartDigraph::Node, SmartDigraph::Node> od1;

    foreach (od1, odlist) {
       nodes2.removeAll(od1.second);
    }

    SmartDigraph::Node start=vehicle->nextPosition();
    //path.push_back(start);
    do
    {
        QPair < SmartDigraph::Node, SmartDigraph::Node> od2;
        QList<QPair<SmartDigraph::Node, double> > locdist;
        foreach (SmartDigraph::Node n2, nodes2) {
            QPair <SmartDigraph::Node, double> loc;
            loc.second=(toPoint(start)-toPoint(n2)).manhattanLength()*40000/360;
            loc.first=n2;
            locdist.append(loc);
        }
        std::sort(locdist.begin(),locdist.end(),comparator3);
        SmartDigraph::Node next_node=locdist.first().first;
        start=next_node;
        nodes2.removeAll(next_node);
        foreach ( od2, odlist) {
           if (next_node==od2.first)
           {
               nodes2.append(od2.second);
           }
        }
        path.push_back(next_node);
    }while (nodes2.size()>0);
     perms.append(path);
     return perms;
 }


VehicleRouter* VehicleRouter::instance(World* wrld)
{
   if(myInstance==NULL && wrld!=NULL)
       myInstance=new VehicleRouter(wrld);
   return myInstance;
}


void VehicleRouter::setWorld(World* wrld)
{
    myWorld=wrld;
}

World* VehicleRouter::world() const
{
    return myWorld;
}

Vehicle* VehicleRouter::route(Customer* cust)
{


    QList<std::pair <Vehicle*, qreal> > distances;
    distances=getDistancesFromCustomer(cust);
    qDebug()<<"There are"<<distances.size()<<"candidates";
    Vehicle* agent;
    Vehicle* matched_agent=NULL;
    for (QList<std::pair <Vehicle*, qreal> >::iterator iter = distances.begin(); iter != distances.end(); ++iter) {
        agent=(*iter).first;


        // Gathering of the destinations of the customers
        QPair <QList <SmartDigraph::Node>,QList < QPair < SmartDigraph::Node, SmartDigraph::Node> > > stop_locations=getStopNodes(agent,cust);
        QList <SmartDigraph::Node> nodes=stop_locations.first;

        QList < QPair < SmartDigraph::Node, SmartDigraph::Node> > odlist=stop_locations.second;

        //get all permutations of the nodes
        QList < QList <SmartDigraph::Node> > perms=getPermutations(nodes);
        //QList < QList <SmartDigraph::Node> > perms=getPermutations2(nodes,agent,odlist);
        //qDebug()<<"There are :"<<perms.size()<<"permutations";

        SmartDigraph::Node agent_position;
        agent_position=agent->nextPosition();


        //collect all possibilities in all_paths, go through all permutations and get the shortest one that visits all nodes
        QList <std::pair <QList <SmartDigraph::Node>,double> > all_paths;


        QList <SmartDigraph::Node> total_path;
        //Create a structure that keeps the already computed routes from one important node to another in order to avoid recomputing
        QMap < QPair<SmartDigraph::Node,SmartDigraph::Node>, std::pair <QList <SmartDigraph::Node>,double> > calculated_segments;
        //Go through all the permutations
        foreach (QList <SmartDigraph::Node> p, perms) {
            if (p.first()!=agent_position){
                p.push_front(agent_position);
            }
            double total_distance=0.0;

            total_path.clear();
            QList <SmartDigraph::Node> current=p;
            if (legitPermutation(current,odlist)){// Check if the origin of the cutomer is before the destination
                //Go through all the nodes that need to be visited in this permutation and get the total distance and path
                foreach (SmartDigraph::Node j, current) {
                    SmartDigraph::Node s=j;
                    int index=current.indexOf(j,0)+1;
                    if (index!=current.size()){
                        SmartDigraph::Node t=current.at(index);
                        QPair<SmartDigraph::Node,SmartDigraph::Node> start_and_end;
                        start_and_end.first=s;
                        start_and_end.second=t;
                        std::pair <QList <SmartDigraph::Node>,double> routing_segment;
                        //Check if the route from t to s is already computed. If not compute it and add it to the already computed routes
                        if (calculated_segments.contains(start_and_end)){
                            routing_segment=calculated_segments[start_and_end];
                        }
                        else
                        {
                            routing_segment=getRoute(s,t,agent->world()->network());
                            calculated_segments.insert(start_and_end, routing_segment);
                        }



                        total_distance=total_distance+routing_segment.second;
                        total_path=total_path+routing_segment.first;
                        total_path.removeLast();
                    }
                    else
                        total_path.append(current.last());
                }
                std::pair <QList <SmartDigraph::Node>,double> new_entry;
                new_entry.first=total_path;
                new_entry.second=total_distance;
                all_paths.append(new_entry);
            }

        }
        //qDebug()<<"Went through the permutations";
        //Sort all paths according to their total distance
        sort(all_paths.begin(),all_paths.end(),comparator2);
        //Go through the list of possible routes starting from the shortest and evaluate if it the other customers can take it.
        std::pair <QList <SmartDigraph::Node>,double> route;
        QList <SmartDigraph::Node> just_route;
        QList <SmartDigraph::Node> just_route_until_pickup;
        QList < QPair <SmartDigraph::Node, double> > route_speed;
        bool found=false;
        foreach (route, all_paths)
        {
            if (canAddCustomer(route.first,agent,cust)){
                    agent->addCustomer(cust);//add the customer to the agent
                    just_route=route.first;//
                    for ( QList <SmartDigraph::Node>::iterator route_iter = just_route.begin();route_iter != just_route.end(); ++route_iter) {
                        QPair <SmartDigraph::Node, double> new_element;
                        new_element.first=(*route_iter);
                        new_element.second=SPEED;
                        route_speed.push_back(new_element);
                    }
                    agent->setRouteAndSpeed(route_speed);//change the route of the agent
                    //cust->setVehicle(agent);
                    int pickup_node_index=just_route.indexOf((cust->origin()));
                    just_route_until_pickup=just_route.mid(0,pickup_node_index);
                    found=true;

            }
            if (found) {matched_agent=agent;break;}
        }
        if (found) {matched_agent=agent;break;}
    }
    return (matched_agent);
// should return the vehicle that will take the customer. if there is no vehicle return NULL
}


