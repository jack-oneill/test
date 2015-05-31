#include "vehicledispatcher.h"
#include "world.h"
#include "utilities.h"
#include "vehiclerouter.h"
#include <QDebug>
#include <cmath>
VehicleDispatcher* VehicleDispatcher::myInstance=NULL;

void VehicleDispatcher::constructHistogram()
{
    if(myWorld==NULL || myGenerator==NULL)
    {
        qDebug()<<"VehicleDispatcher::constructHistogram() Undefined world or generator. Cannot dispatch idle vehicles.";
        return;
    }
    QVector<CustomerGeneratorSample*> samples = myGenerator->samples();
    QVector<Neighborhood*> neighborhoods = myWorld->network()->neigborhoods();
    myDispatchedVehicles.clear();
    myLocatedVehicles.clear();
    myDispatchedVehicles.resize(neighborhoods.size());
    myTabuLists.clear();
    myLocatedVehicles.resize(neighborhoods.size());
    for(int i=0;i<neighborhoods.size();++i)
        myLocatedVehicles[i]=0;
    for(int i=0;i<samples.size();++i)
    {
        uint64_t time =  samples[i]->start();
        if(!myHistogram.contains(time))
        {
            myHistogram[time].resize(neighborhoods.size());
            for(int j=0;j<neighborhoods.size();++j)
            {
                myHistogram[time][j]=0;
            }
        }
        myHistogram[time][samples[i]->source()]+=1;
        myValidNeighborhoods.insert(samples[i]->source());
    }

}

VehicleDispatcher::VehicleDispatcher() :
    QObject(NULL)
{
    myWorld=NULL;
    myGenerator=NULL;
}

void VehicleDispatcher::setup(World *world, CustomerGeneratorOffline *generator)
{

    myWorld=world;
    myGenerator=generator;
    myHistogram.clear();
    myValidNeighborhoods.clear();
    constructHistogram();
}
VehicleDispatcher* VehicleDispatcher::instance()
{
    if(myInstance==NULL)
        myInstance=new VehicleDispatcher();
    return myInstance;
}


void VehicleDispatcher::prune()
{
    if(myWorld==NULL)
        return;
    uint64_t time = myWorld->time();
    for(int i=0;i<myDispatchedVehicles.size();++i)
    {
        QList<uint64_t> toRemove;
        for(QList<uint64_t>::iterator it = myDispatchedVehicles[i].begin();it!=myDispatchedVehicles[i].end();++it)
        {
            if(time-(*it)>=1200)//Dispatched 20 min ago
                toRemove.append(*it);
        }
        for(QList<uint64_t>::iterator it = toRemove.begin();it!=toRemove.end();++it)
            myDispatchedVehicles[i].removeAll(*it);
    }
}

unsigned VehicleDispatcher::integrate(uint64_t start,uint64_t end,unsigned n)
{
    unsigned result=0;
    for(QMap<uint64_t,QVector<unsigned> >::iterator it = myHistogram.begin();it!=myHistogram.end();++it)
    {
        if(it.key()>=start && it.key()<=end)
        {
           result+=it.value()[n];
        }
    }
    return result;
}

double VehicleDispatcher::score(Vehicle* veh,const unsigned& nId)
{
    if(!myValidNeighborhoods.contains(nId))
        return -1;
    if(veh==NULL)
    {
        qDebug()<<"VehicleDispatcher::score() No vehicle specified";
        return 0;
    }
    if(myWorld==NULL)
    {
        qDebug()<<"VehicleDispatcher::constructHistogram() Undefined world. Cannot dispatch idle vehicles.";
        return 0;
    }
    lemon::SmartDigraph::Node vnode= veh->position(myWorld->time()).first;
    Neighborhood* neigh = myWorld->network()->neigborhoods()[nId];
    QPointF vpos = (*myWorld->network()->positionMap())[vnode];
    double distance = Utilities::dist(vpos,neigh->center())/1000;
    distance+=Utilities::dist(neigh->center(),myWorld->network()->center())/1000;
    if(distance==0)
        distance=0.05;//50 meters
    uint64_t start=myWorld->time();
    uint64_t end=0;
    uint64_t approxTimeAtPoint = distance*1.5/SPEED+start;
    if(approxTimeAtPoint-1200>start)
    {
        start=approxTimeAtPoint-1200;
    }
    end=start+2400;
    unsigned customers=integrate(start,end,nId);
    /*if(customers==0)
        return 0;*/
    double customer_score = customers;
    double vehicle_score = myAverageSeats*(myLocatedVehicles[nId]*0.5+myDispatchedVehicles[nId].size()*.75);
    return  30*customer_score/(0.1+vehicle_score*distance);

}

int VehicleDispatcher::locateVehicles(Vehicle* veh)
{
    QList<Vehicle*> vehicles = myWorld->vehicles();
    QVector<Neighborhood*> neighborhoods = myWorld->network()->neigborhoods();
    for(int i=0;i<myLocatedVehicles.size();++i)
        myLocatedVehicles[i]=0;
    int vneigh=-1;
    myAverageSeats=0;
    for(QList<Vehicle*>::iterator it = vehicles.begin();it!=vehicles.end();++it)
    {
        myAverageSeats+=(*it)->capacity();
        for(int i=0;i<neighborhoods.size();++i)
        {
            if(neighborhoods[i]->contains((*it)->position(myWorld->time()).first))
                myLocatedVehicles[i]++;
            if((*it)==veh)
                vneigh=i;
        }
    }
    myAverageSeats/=vehicles.size();
    return vneigh;

}

void VehicleDispatcher::route(Vehicle * veh)
{
    if(veh==NULL || veh->routeAndSpeed().size()!=0 ||myWorld==NULL || myGenerator==NULL)
        return;
    prune();
    int vneigh = locateVehicles(veh);
    QVector<Neighborhood*> neighborhoods = myWorld->network()->neigborhoods();
    if(neighborhoods.size()==0)
    {
        qDebug()<<"VehicleDispatcher::route(Vehicle*) No neighborhoods found.";
        return;
    }
    double nscore=0;
    double tscore=0;
    Neighborhood* selected = neighborhoods[0];
    int selected_index=0;
    for(int i=0;i<neighborhoods.size();++i)
    {
        if(!myTabuLists[veh].contains(i) && neighborhoods[i]->nodes().size()>0 && i!=vneigh)
        {
            tscore = score(veh,(unsigned)i);
            if(nscore<tscore)
            {
                nscore=tscore;
                selected = neighborhoods[i];
                selected_index=i;
            }
        }
    }
    myTabuLists[veh].push_back(selected_index);
    myDispatchedVehicles[selected_index].append(myWorld->time());
    //qDebug()<<"Selected "+selected->name();
    if(myTabuLists[veh].size()>4)
        myTabuLists[veh].pop_front();
    QList<lemon::SmartDigraph::Node> troute =  VehicleRouter::instance(myWorld)->getRoute(veh->position(myWorld->time()).first,selected->centralNode(),myWorld->network()).first;
    QList<QPair<lemon::SmartDigraph::Node, double> > sroute;
    for(int i=0;i<troute.size();++i)
    {
        sroute.push_back(QPair<lemon::SmartDigraph::Node,double>(troute[i],SPEED));
    }
    veh->setRouteAndSpeed(sroute);
}
