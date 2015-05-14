#include "agentfactory.h"


AgentFactory* AgentFactory::myInstance=NULL;
AgentFactory::AgentFactory()
{
    myVehicleMaxId=0;
    myCustomerMaxId=0;
}
AgentFactory* AgentFactory::instance()
{
    if(myInstance==NULL)
        myInstance=new AgentFactory();
    return myInstance;
}
void AgentFactory::reset()
{
    myVehicleMaxId=0;
    myCustomerMaxId=0;
}

uint64_t AgentFactory::newVehicleId()
{
    return myVehicleMaxId++;
}

uint64_t AgentFactory::newCustomerId()
{
    return myCustomerMaxId++;
}

Vehicle* AgentFactory::newVehicle(const QString& name,const unsigned& cap, const lemon::SmartDigraph::Node& location,World* world )
{
    return new Vehicle(newVehicleId(),cap,name,location,world);
}

Customer* AgentFactory::newCustomer( const QString& name, const lemon::SmartDigraph::Node& start, const lemon::SmartDigraph::Node&  end, World* world)
{
    return new Customer(newCustomerId(),name,start,end,world);
}
