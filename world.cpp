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
QList<Customer*> World::customersInNeighborhood(const int& index) const
{
    QMap<int,QList<Customer*> >::const_iterator hit=myNeighToCustomers.constFind(index);
    if(hit==myNeighToCustomers.constEnd())
        return QList<Customer*>();
    return hit.value();
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
    int neigh = cust->neighborhoodId();
    if(neigh>=0)
    {
        myNeighToCustomers[neigh].append(cust);
        myCustomerToNeigh[cust]=neigh;
    }
    emit customerAdded(cust);
}
void World::removeCustomer(Customer * cust)
{
    QMap<Customer*,int>::iterator neighIt = myCustomerToNeigh.find(cust);
    if(neighIt!=myCustomerToNeigh.end())
    {
        int neigh = neighIt.value();
        myNeighToCustomers.remove(neigh);
        myCustomerToNeigh.erase(neighIt);
    }
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
void World::takeCustomer(Customer* cust)
{
   emit customerTaken(cust);
}

void World::clear()
{
    QMap<Customer*,int>::iterator neighIt;
    int neigh=-1;
    for(QMap<uint64_t,Customer*>::iterator it = myCustomers.begin();it!=myCustomers.end();++it)
    {
        neighIt = myCustomerToNeigh.find(it.value());
        if(neighIt!=myCustomerToNeigh.end())
        {
            neighIt.value();
            myNeighToCustomers.remove(neigh);
            myCustomerToNeigh.erase(neighIt);
        }
        emit customerDestroyed(it.value());
        delete it.value();
    }
    myCustomers.clear();
    for(QMap<uint64_t,Vehicle*>::iterator it = myVehicles.begin();it!=myVehicles.end();++it)
    {
        emit vehicleDestroyed(it.value());
        delete it.value();
    }
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
