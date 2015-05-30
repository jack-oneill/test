#include "event.h"
#include "vehicledispatcher.h"
#include "utilities.h"
#include <QSet>
#include <QDebug>
#include "customer.h"
#include "vehicle.h"
#include "simulationkernel.h"
#include "customergenerationprocess.h"
#include "customergenerator.h"
#include "agentfactory.h"
#include "vehiclerouter.h"
#include "gui/logger.h"
#include "outputgenerator.h"
#include "scoreboard.h"
#include <stdlib.h>


Event::Event(Agent* ag,uint64_t tim ,SimulationKernel* ker )
{
    myAgent=ag;
    myTime=tim;
    myKernel=ker;
    ker->pushEvent(this);
}

EventType Event::type() const
{
    return myType;
}

uint64_t Event::time() const
{
    return myTime;
}

Agent* Event::agent() const
{
    return myAgent;
}

EventStartSimulation::EventStartSimulation(SimulationKernel* ker ) : Event(NULL,0,ker)
{
    myType = t_EventStartSimulation;
}

void EventStartSimulation::execute()
{
    QList<Vehicle*> vehicles  = myKernel->world()->vehicles();
    for(int i=0;i<vehicles.size();++i)
    {
        new EventRepositionVehicle(vehicles[i],3000,myKernel);
    }
    /*uint64_t tim=CustomerGenerationProcess::instance()->next();
    QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node>  nodes(lemon::INVALID,lemon::INVALID);
    while(nodes.first==nodes.second)
        nodes=CustomerGenerator::instance(myKernel->world())->generate();
    lemon::SmartDigraph::Node origin=nodes.first;
    lemon::SmartDigraph::Node destination=nodes.second;
    Customer* ncust = AgentFactory::instance()->newCustomer("Customer",origin,destination,this->myKernel->world());
    new EventShowUp(ncust,myTime+tim,myKernel);*/

}

EventMove::EventMove(Vehicle* veh,uint64_t tim,unsigned index, unsigned total,SimulationKernel* ker) : Event(veh,tim,ker)
{
    myIndex=index;
    myTotal=total;
    myType=t_EventMove;
}

QSet<Customer*> EventMove::customersWithinReach()
{
    Vehicle* veh = (Vehicle*)myAgent;
    World* world = veh->world();
    QVector<Neighborhood*> neighborhoods= world->network()->neigborhoods();
    RoutingNetwork::PositionMap* pmap = world->network()->positionMap();
    QPointF vpos = (*pmap)[veh->nextPosition()];
    QSet<Customer*> rval;
    double mdist = world->network()->getMaxNeighborhoodRadius()+2000;
    for(int i=0;i<neighborhoods.size();++i)
    {
        if(mdist>=Utilities::dist(vpos,neighborhoods[i]->center()))
        {
           QList<Customer*> ncust = world->customersInNeighborhood(i);
           for(int j=0;j<ncust.size();++j)
           {
                if(Utilities::dist((*pmap)[ncust[j]->position(0).first],vpos)<=2000)
                {
                    rval.insert(ncust[j]);
                }
           }
        }
    }
    return rval;
}

void EventMove::execute()
{
    Vehicle* vehicle = (Vehicle*) myAgent;
    vehicle->move(myTime);
    Logger::instance()<<50<<myTime<<"Vehicle "+QString::number(vehicle->id())+" moved. ["+QString::number(myIndex)+"/"+QString::number(myTotal)+"]";
    QString subline;
    QList<Customer*> lcust=vehicle->customers().toList();
    for(int i=0;i<std::max((int)vehicle->capacity(),2);++i)
    {
       if(lcust.size()>i)
       {
           if(!lcust[i]->vehicle()==NULL)
               subline+= QString::number(lcust[i]->id())+",";
           else
               subline+= "-1,";
       }
       else
       {
           subline+= "-1,";
       }
    }
    OutputGenerator::instance()->write(QString::number(vehicle->id()) + ","+
                           QString::number(myTime/60.0)+","+
                           QString::number((*this->agent()->world()->network()->arcIdMap())[vehicle->arc()])+","+
                           subline+
                           "0" );
    /*if(vehicle->capacity()>vehicle->customers().size())
    {
        QSet<Customer*> ncust = customersWithinReach();
        //New customers within reach
        QSet<Customer*> ncust_new = ncust-vehicle->customersWithinReach();
        vehicle->setCustomersWithinReach(ncust);
        QVector<Customer*> valid;
        for(QSet<Customer*>::iterator it = ncust_new.begin();it!=ncust_new.end();++it)
        {
            if(!myKernel->existsEvent(myTime,t_EventSnooze,*it) && (*it)->requestTime()+1800>myTime && (*it)->hasVehicle()==false)
                valid.append(*it);
        }
        for(int i=0;i<valid.size();++i)
        {
            new EventSnooze(valid[i],0,myTime,myKernel);
        }
    }*/
    //Reroute the vehicle to a neighborhood
    if(myIndex==myTotal)
    {
        vehicle->setRouteAndSpeed(QList<QPair<lemon::SmartDigraph::Node,double> >(),0);
        new EventRepositionVehicle(vehicle,myTime+300,myKernel);
    }
}

EventPickUp::EventPickUp(Customer* cust, Vehicle* veh,uint64_t tim,SimulationKernel* ker) : Event(veh,tim,ker)
{
    myType=t_EventPickUp;
    myCustomer=cust;
}

Customer* EventPickUp::customer()
{
    return myCustomer;
}

void EventPickUp::execute()
{
    Vehicle* vehicle = (Vehicle*) myAgent;
    vehicle->addCustomer(myCustomer);
    myCustomer->setVehicle(vehicle);
    myCustomer->setPickUpTime(myTime);
    myCustomer->world()->takeCustomer(myCustomer);
    Logger::instance()<<25<<myTime<<"Vehicle "+QString::number(vehicle->id())+" picked up customer "+QString::number(myCustomer->id())+" at node "
                        +QString::number((*myAgent->world()->network()->nodeIdMap())[myCustomer->origin()]);
    QString subline;
    QList<Customer*> lcust=vehicle->customers().toList();
    for(int i=0;i<std::max(2,(int)vehicle->capacity());++i)
    {
       if(lcust.size()>i)
       {
           if(!lcust[i]->vehicle()==NULL)
               subline+= QString::number(lcust[i]->id())+",";
           else
               subline+= "-1,";
       }
       else
       {
           subline+= "-1,";
       }
    }
    OutputGenerator::instance()->write(QString::number(vehicle->id()) + ","+
                           QString::number(myTime/60.0)+","+
                           QString::number((*this->agent()->world()->network()->arcIdMap())[vehicle->arc()])+","+
                           subline+
                           "1" );
}

EventDropOff::EventDropOff(Customer* cust, Vehicle* veh,uint64_t tim,SimulationKernel* ker) : Event(veh,tim,ker)
{
    myType=t_EventDropOff;
    myCustomer=cust;
}

QSet<Customer*> EventDropOff::customersWithinReach()
{
    Vehicle* veh = (Vehicle*)myAgent;
    World* world = veh->world();
    QVector<Neighborhood*> neighborhoods= world->network()->neigborhoods();
    RoutingNetwork::PositionMap* pmap = world->network()->positionMap();
    QPointF vpos = (*pmap)[veh->nextPosition()];
    QSet<Customer*> rval;
    double mdist = world->network()->getMaxNeighborhoodRadius()+2000;
    for(int i=0;i<neighborhoods.size();++i)
    {
        if(mdist>=Utilities::dist(vpos,neighborhoods[i]->center()))
        {
           QList<Customer*> ncust = world->customersInNeighborhood(i);
           for(int j=0;j<ncust.size();++j)
           {
                if(Utilities::dist((*pmap)[ncust[j]->position(0).first],vpos)<=2000)
                {
                    rval.insert(ncust[j]);
                }
           }
        }
    }
    return rval;
}


Customer* EventDropOff::customer()
{
    return myCustomer;
}

void EventDropOff::execute()
{
    QMutexLocker lock(&myKernel->viewRefreshMutex());
    Vehicle* vehicle = (Vehicle*) myAgent;
    vehicle->removeCustomer(myCustomer);
    Logger::instance()<<25<<myTime<<"Vehicle "+QString::number(vehicle->id())+" delivered customer "+QString::number(myCustomer->id())+" at node "
                        +QString::number((*myAgent->world()->network()->nodeIdMap())[myCustomer->origin()]);
    OutputGenerator::instance()->writeCust(QString::number(myCustomer->id()) + ","+
                           QString::number(myTime/60.0)+","+
                           QString::number(myCustomer->pickUpTime()/60.0)+","+
                           QString::number(myCustomer->optimalTime()/60.0) +","+
                           QString::number((myTime-myCustomer->pickUpTime())/60.0) +","+
                           QString::number((myCustomer->pickUpTime()-myCustomer->requestTime())/60.0)+","
                           "3" );
    ScoreBoard::instance()->last()->addDelivered(myTime-myCustomer->pickUpTime(),myCustomer->optimalTime(),myCustomer->pickUpTime()-myCustomer->requestTime());
    myCustomer->world()->removeCustomer(myCustomer);
    QString subline;
    QList<Customer*> lcust=vehicle->customers().toList();
    for(int i=0;i<std::max(2,(int)vehicle->capacity());++i)
    {
       if(lcust.size()>i)
       {
           if(!lcust[i]->vehicle()==NULL)
               subline+= QString::number(lcust[i]->id())+",";
           else
               subline+= "-1,";
       }
       else
       {
           subline+= "-1,";
       }
    }
    OutputGenerator::instance()->write(QString::number(vehicle->id()) + ","+
                           QString::number(myTime/60.0)+","+
                           QString::number((*this->agent()->world()->network()->arcIdMap())[vehicle->arc()])+","+
                           subline+
                           "2" );
    /*if(vehicle->capacity()>vehicle->customers().size())
    {
        QSet<Customer*> ncust = customersWithinReach();
        vehicle->setCustomersWithinReach(ncust);
        QVector<Customer*> valid;
        for(QSet<Customer*>::iterator it = ncust.begin();it!=ncust.end();++it)
        {
            if(!myKernel->existsEvent(myTime,t_EventSnooze,*it) &&(*it)->requestTime()+1800!=myTime && (*it)->hasVehicle()==false )
                valid.append(*it);
        }
        for(int i=0;i<valid.size();++i)
        {
            new EventSnooze(valid[i],0,myTime,myKernel);
        }
    }*/
}

EventShowUp::EventShowUp(Customer* cust,uint64_t tim,SimulationKernel* ker) : Event(cust,tim,ker)
{
    myType=t_EventShowUp;
}
void EventShowUp::assignVehicle()
{
    VehicleRouter* router = VehicleRouter::instance(myAgent->world());
    if(router==NULL)
        return;
    Vehicle* veh = router->route((Customer*)myAgent);
    if(veh==NULL)
    {
        Logger::instance()<<50<<myTime<<"No vehicle for customer "+QString::number(((Customer*)myAgent)->id())+". Waiting for "+QString::number(SNOOZETIME)+" seconds.";
        if(myTime-((Customer*)myAgent)->requestTime()>=1800)//Half an hour
            new EventDisappear((Customer*)myAgent,myTime+1800,myKernel);
        else
            new EventSnooze((Customer*)myAgent,SNOOZETIME,myTime+SNOOZETIME,myKernel);
        //if(myTime==((Customer*)myAgent)->requestTime() && this->type()==t_EventShowUp)
    }
    else
    {
        Logger::instance()<<25<<myTime<<"Customer "+QString::number(myAgent->id())+" assigned to "+QString::number(veh->id())+".";
        QList<QPair<lemon::SmartDigraph::Node,double> > route=veh->routeAndSpeed();
        myKernel->removeEvents(veh);
        myKernel->removeEvents(myAgent);
        uint64_t tim=myTime;
        QSet<Customer*> customers = veh->customers();
        QMap<lemon::SmartDigraph::Node,QList<Customer*> > destToCust;
        QMap<lemon::SmartDigraph::Node,QList<Customer*> > origToCust;
        QMap<lemon::SmartDigraph::Node,QList<Customer*> >::iterator it;
        QSet<Customer*> hasPickUpEvent;
        QSet<Customer*> hasDropOffEvent;
        for(QSet<Customer*>::iterator sit=customers.begin();sit!=customers.end();++sit)
        {
           if((*sit)->vehicle()!=NULL)
               hasPickUpEvent.insert(*sit);
            destToCust[(*sit)->destination()].append(*sit);
            origToCust[(*sit)->origin()].append(*sit);
        }
        it = origToCust.find(route[0].first);
        if(it!=origToCust.end())
        {
            for(int j=0;j<it.value().size();++j)
            {
                if(!hasPickUpEvent.contains(it.value()[j]))
                {
                    new EventPickUp(it.value()[j],veh,tim,myKernel);
                    hasPickUpEvent.insert(it.value()[j]);
                }
            }
        }
        it=destToCust.find(route[0].first);
        if(it!=destToCust.end())
        {
            for(int j=0;j<it.value().size();++j)
            {
                if(hasPickUpEvent.contains(it.value()[j]) && !hasDropOffEvent.contains(it.value()[j]))
                {
                    new EventDropOff(it.value()[j],veh,tim,myKernel);
                    hasDropOffEvent.insert(it.value()[j]);
                }
            }
        }
        for(int i=1;i<route.size();++i)
        {
            lemon::SmartDigraph* graph = myAgent->world()->network()->graph();
            double dist = 0;
            for(lemon::SmartDigraph::OutArcIt oait(*graph,route[i-1].first);oait!=lemon::INVALID;++oait)
            {
               if(graph->target(oait)==route[i].first)
               {
                  dist = (*myAgent->world()->network()->distanceMap())[oait];
                  break;
               }
            }
            tim+=(uint64_t)ceil(dist*1000/route[i].second);
            //if(veh->id()==16)
            //  qDebug()<<QString::number(i)+" Info : "+QString::number(tim)+", Speed: "+QString::number(route[i].second)+", Dist: "+QString::number(dist)+", Vehicle: "+QString::number(veh->id());
            new EventMove(veh,tim,(unsigned)i,(unsigned)route.size()-1,myKernel);
            it=destToCust.find(route[i].first);
            if(it!=destToCust.end())
            {
                for(int j=0;j<it.value().size();++j)
                {
                    if(hasPickUpEvent.contains(it.value()[j]) && !hasDropOffEvent.contains(it.value()[j]))
                    {
                        new EventDropOff(it.value()[j],veh,tim,myKernel);
                        hasDropOffEvent.insert(it.value()[j]);
                    }
                }
            }
            it = origToCust.find(route[i].first);
            if(it!=origToCust.end())
            {
                for(int j=0;j<it.value().size();++j)
                {
                    if(!hasPickUpEvent.contains(it.value()[j]))
                    {
                        new EventPickUp(it.value()[j],veh,tim,myKernel);
                        hasPickUpEvent.insert(it.value()[j]);
                    }
                }
            }
        }
        if(hasDropOffEvent.size()!=hasPickUpEvent.size() || hasDropOffEvent.size()!=customers.size())
        {
            qDebug()<<"Unmatched pickup and dropoff events!";
        }
    }

}

void EventShowUp::execute()
{
    Customer* customer=(Customer*)myAgent;
    customer->setRequestTime(time());
    myKernel->world()->addCustomer(customer);
    Logger::instance()<<25<<myTime<<("Customer "+QString::number(customer->id())+" appeared.");
    double idist = VehicleRouter::instance(customer->world())->getRoute(customer->origin(),customer->destination(),customer->world()->network()).second;
    customer->setIdealDistance(idist);
    customer->setOptimalTime((uint64_t)ceil(1000*idist/SPEED));

    /*
    uint64_t tim=CustomerGenerationProcess::instance()->next();
    QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node>  nodes(lemon::INVALID,lemon::INVALID);
    while(nodes.first==nodes.second)
        nodes=CustomerGenerator::instance(customer->world())->generate();
    lemon::SmartDigraph::Node origin=nodes.first;
    lemon::SmartDigraph::Node destination=nodes.second;
    double idist = VehicleRouter::instance(customer->world())->getRoute(customer->origin(),customer->destination(),customer->world()->network()).second;
    customer->setIdealDistance(idist);
    Customer* ncust = AgentFactory::instance()->newCustomer("Customer",origin,destination,this->myKernel->world());*/
    assignVehicle();
    //new EventShowUp(ncust,myTime+tim,myKernel);
}


EventDisappear::EventDisappear(Customer* cust,uint64_t tim,SimulationKernel* ker) : Event(cust,tim,ker)
{
    myType=t_EventDisappear;
}


void EventDisappear::execute()
{
    Customer* customer = (Customer*)myAgent;
    if(customer->hasVehicle())
        return;
    Logger::instance()<<50<<myTime<<"Customer "+QString::number(customer->id())+" disappeared.";
    OutputGenerator::instance()->writeCust(QString::number(customer->id()) + ","+
                           QString::number(myTime/60.0)+","+
                           "-1,"+
                           QString::number(customer->optimalTime()/60.0)+","
                           "-1,"+
                           "-1,"+
                           "3" );
    ScoreBoard::instance()->last()->addGiveUp();
    customer->world()->removeCustomer(customer);
}

EventSatisfactionDown::EventSatisfactionDown(unsigned st,Customer* cust,uint64_t tim,SimulationKernel* ker) : Event(cust,tim,ker)
{
    myStep=st;
    myType=t_EventSatisfactionDown;
}

unsigned EventSatisfactionDown::step()
{
    return myStep;
}

void EventSatisfactionDown::execute()
{
    Customer* customer = (Customer*)myAgent;
    Logger::instance()<<50<<myTime<<"Customer "+QString::number(customer->id())+" satisfaction down.";
    if(customer->satisfaction()<myStep)
        customer->setSatisfaction(0);
    else
        customer->setSatisfaction(customer->satisfaction()-myStep);
}

EventSnooze::EventSnooze(Customer* cust,uint64_t delta,uint64_t tim,SimulationKernel* ker) : EventShowUp(cust,tim,ker)
{
    myDelta=delta;
    myType=t_EventSnooze;
}

void EventSnooze::execute()
{
    Customer* cust = (Customer*)myAgent;
    Logger::instance()<<50<<myTime<<"Retrying customer "+QString::number(cust->id());
    //cust->setRequestTime(cust->requestTime()+myDelta);
    assignVehicle();
}


EventRepositionVehicle::EventRepositionVehicle(Vehicle* veh,uint64_t tim,SimulationKernel* ker) : Event(veh,tim,ker)
{
}

void EventRepositionVehicle::execute()
{
    Vehicle* veh = (Vehicle*)myAgent;
    uint64_t tim = myTime;
    if(veh->routeAndSpeed().size()==0)
    {
        Logger::instance()<<50<<myTime<<"Vehicle "+QString::number(veh->id())+" will be repositioned.";
        VehicleDispatcher::instance()->route(veh);
        lemon::SmartDigraph* graph = myAgent->world()->network()->graph();
        double dist = 0;
        QList<QPair<lemon::SmartDigraph::Node,double> > route = veh->routeAndSpeed();
        for(int i=1;i<route.size();++i)
        {
            for(lemon::SmartDigraph::OutArcIt oait(*graph,route[i-1].first);oait!=lemon::INVALID;++oait)
            {
               if(graph->target(oait)==route[i].first)
               {
                  dist = (*myAgent->world()->network()->distanceMap())[oait];
                  break;
               }
            }
            tim+=(uint64_t)ceil(dist*1000/route[i].second);
            new EventMove(veh,tim,(unsigned)i,(unsigned)route.size()-1,myKernel);
        }
        if(route.size()<=1)
        {
            new EventRepositionVehicle(veh,myTime+3000,myKernel);
        }
    }
}
