#ifndef AGENTFACTORY_H
#define AGENTFACTORY_H
#include "customer.h"
#include "vehicle.h"
#include "simulationkernel.h"
#include <QString>
class AgentFactory
{
protected:
    uint64_t myVehicleMaxId;
    uint64_t newVehicleId();
    uint64_t myCustomerMaxId;
    uint64_t newCustomerId();
    AgentFactory();
    static AgentFactory* myInstance;
public:
    static AgentFactory* instance();
    Vehicle* newVehicle(const QString&,const unsigned&,const lemon::SmartDigraph::Node&,World*);
    Customer* newCustomer(const QString&,const lemon::SmartDigraph::Node&,const lemon::SmartDigraph::Node&,World*);
    void reset();
};

#endif // AGENTFACTORY_H
