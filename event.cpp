#include "event.h"
#include "customer.h"
#include "vehicle.h"
#include "simulationkernel.h"
#include "customergenerationprocess.h"
#include "customergenerator.h"
#include "agentfactory.h"
#include "vehiclerouter.h"
#include "gui/logger.h"
#include "outputgenerator.h"
#include <stdlib.h>


Event::Event(Agent* ag,uint64_t tim ,SimulationKernel* ker )
{
    myAgent=ag;
    myTime=tim;
    myKernel=ker;
    ker->pushEvent(this);
}

EventType Event::type()
{
    return myType;
}

uint64_t Event::time()
{
    return myTime;
}

Agent* Event::agent()
{
    return myAgent;
}

EventStartSimulation::EventStartSimulation(SimulationKernel* ker ) : Event(NULL,0,ker)
{
    myType = t_EventStartSimulation;
}

void EventStartSimulation::execute()
{
    /*uint64_t tim=CustomerGenerationProcess::instance()->next();
    QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node>  nodes(lemon::INVALID,lemon::INVALID);
    while(nodes.first==nodes.second)
        nodes=CustomerGenerator::instance(myKernel->world())->generate();
    lemon::SmartDigraph::Node origin=nodes.first;
    lemon::SmartDigraph::Node destination=nodes.second;
    Customer* ncust = AgentFactory::instance()->newCustomer("Customer",origin,destination,this->myKernel->world());
    new EventShowUp(ncust,myTime+tim,myKernel);*/

}

EventMove::EventMove(Vehicle* veh,uint64_t tim,SimulationKernel* ker) : Event(veh,tim,ker)
{
    myType=t_EventMove;
}

void EventMove::execute()
{
    Vehicle* vehicle = (Vehicle*) myAgent;
    vehicle->move(myTime);
    Logger::instance()<<50<<myTime<<"Vehicle "+QString::number(vehicle->id())+" moved.";
    QString subline;
    QList<Customer*> lcust=vehicle->customers().toList();
    for(int i=0;i<std::max((int)vehicle->capacity(),2);++i)
    {
       if(lcust.size()>i)
       {
           if(!lcust[i]->vehicle()!=NULL)
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

Customer* EventDropOff::customer()
{
    return myCustomer;
}

void EventDropOff::execute()
{
    Vehicle* vehicle = (Vehicle*) myAgent;
    vehicle->removeCustomer(myCustomer);
    Logger::instance()<<25<<myTime<<"Vehicle "+QString::number(vehicle->id())+" delivered customer "+QString::number(myCustomer->id())+" at node "
                        +QString::number((*myAgent->world()->network()->nodeIdMap())[myCustomer->origin()]);
    QString fill;
    for(int i=0;i<myKernel->world()->vehicles()[0]->capacity()-2;++i)
    {
       fill+="0,";
    }
    OutputGenerator::instance()->write(QString::number(myCustomer->id()) + ","+
                           QString::number(myTime/60.0)+","+
                           QString::number(myCustomer->pickUpTime()/60)+","+
                           QString::number(myCustomer->optimalTime()/60) +","+
                           QString::number((myTime-myCustomer->pickUpTime())/60) +","+
                           fill+
                           "3" );
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
        new EventSnooze((Customer*)myAgent,SNOOZETIME,myTime+SNOOZETIME,myKernel);
    }
    else
    {
        Logger::instance()<<25<<myTime<<"Customer "+QString::number(myAgent->id())+" assigned to "+QString::number(veh->id())+".";
        QList<QPair<lemon::SmartDigraph::Node,double> > route=veh->routeAndSpeed();
        myKernel->removeEvents(veh);
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
            new EventMove(veh,tim,myKernel);
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
    }
}

void EventShowUp::execute()
{
    Customer* customer=(Customer*)myAgent;
    customer->setRequestTime(time());
    myKernel->world()->addCustomer(customer);
    Logger::instance()<<25<<myTime<<("Customer "+QString::number(customer->id())+" appeared.");
    QString fill;
    for(int i=0;i<myKernel->world()->vehicles()[0]->capacity()-2;++i)
    {
       fill+="0,";
    }
    OutputGenerator::instance()->write(QString::number(customer->id()) + ","+
                           QString::number(myTime/60.0)+","+
                           "-1,"+
                           "-1,"+
                           "-1,"+
                           fill +
                           "3" );
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
    double idist = VehicleRouter::instance(customer->world())->getRoute(customer->origin(),customer->destination(),customer->world()->network()).second;
    customer->setIdealDistance(idist);
    customer->setOptimalTime(idist/SPEED);
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
    Logger::instance()<<50<<myTime<<"Customer "+QString::number(customer->id())+" disappeared.";
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
