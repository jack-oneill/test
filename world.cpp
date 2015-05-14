#include "world.h"
#include "simulationkernel.h"

World::World(const QString& file,QObject* parent) :QObject(parent)
{
    myNetwork=new RoutingNetwork(file);
    myKernel=NULL;
}

World::~World()
{
    if(myNetwork)
        delete myNetwork;
}

void World::setKernel(SimulationKernel* kernel)
{
    myKernel=kernel;
}

SimulationKernel* World::kernel() const
{
    return myKernel;
}

QList<Vehicle*> World::vehicles()
{
    QList<Vehicle*> list;
    for(QMap<uint64_t,Vehicle*>::iterator it = myVehicles.begin();it!=myVehicles.end();++it )
    {
       list.append(it.value());
    }
    return list;
}

QList<Customer*> World::customers()
{
    QList<Customer*> list;
    for(QMap<uint64_t,Customer*>::iterator it = myCustomers.begin();it!=myCustomers.end();++it )
    {
       list.append(it.value());
    }
    return list;
}

uint64_t World::time() const
{
    if(myKernel)
        return myKernel->time();
    return 0;
}

QList<Event*> World::initialize()
{
    return QList<Event*>();
}

void World::addVehicle(Vehicle* vhl)
{
    myVehicles[vhl->id()]=vhl;
    emit vehicleAdded(vhl);
}

void World::addCustomer(Customer* cust)
{
    myCustomers[cust->id()]=cust;
    emit customerAdded(cust);
}
void World::removeCustomer(Customer * cust)
{
    myCustomers.remove(cust->id());
    emit customerDestroyed(cust);
    delete cust;
}

void World::removeVehicle(Vehicle *veh)
{
    myVehicles.remove(veh->id());
    emit vehicleDestroyed(veh);
    delete veh;

}
void World::clear()
{
    for(QMap<uint64_t,Customer*>::iterator it = myCustomers.begin();it!=myCustomers.end();++it)
        delete it.value();
    myCustomers.clear();
    for(QMap<uint64_t,Vehicle*>::iterator it = myVehicles.begin();it!=myVehicles.end();++it)
        delete it.value();
    myVehicles.clear();
}
bool World::valid()
{
   return myNetwork->valid();
}


RoutingNetwork* World::network() const
{
    return myNetwork;
}
