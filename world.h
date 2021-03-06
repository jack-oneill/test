#ifndef WORLD_H
#define WORLD_H

#include "vehicle.h"
#include "customer.h"
#include <QMap>
#include <QObject>
#include "routingnetwork.h"
#include "event.h"
class SimulationKernel;
class World : public QObject
{
    Q_OBJECT
private:
    QMap<uint64_t,Vehicle*> myVehicles;
    QMap<uint64_t,Customer*> myCustomers;
    QMap<int,QList<Customer*> > myNeighToCustomers;
    QMap<Customer*,int > myCustomerToNeigh;
    RoutingNetwork* myNetwork;
    SimulationKernel* myKernel;
public:
    World(const QString&,QObject* parent=NULL);
    ~World();
    void setKernel(SimulationKernel*);
    SimulationKernel* kernel() const;
    QList<Vehicle*> vehicles();
    QList<Customer*> customers();
    QList<Customer*> customersInNeighborhood(const int&) const;
    uint64_t time() const;
    QList<Event*> initialize();
    void addVehicle(Vehicle*);
    void addCustomer(Customer*);
    void takeCustomer(Customer* cust);
    bool valid();
    RoutingNetwork* network() const;
signals:
    void customerAdded(Customer*);
    void vehicleAdded(Vehicle*);
    void customerDestroyed(Agent*);
    void vehicleDestroyed(Agent*);
    void customerTaken(Customer*);
public slots:
    void clear();
    void removeCustomer(Customer*);
    void removeVehicle(Vehicle*);
};

#endif // WORLD_H
