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
    RoutingNetwork* myNetwork;
    SimulationKernel* myKernel;
public:
    World(const QString&,QObject* parent=NULL);
    ~World();
    void setKernel(SimulationKernel*);
    SimulationKernel* kernel() const;
    QList<Vehicle*> vehicles();
    QList<Customer*> customers();
    uint64_t time() const;
    QList<Event*> initialize();
    void addVehicle(Vehicle*);
    void addCustomer(Customer*);
    void removeCustomer(Customer*);
    void removeVehicle(Vehicle*);
    void clear();
    bool valid();
    RoutingNetwork* network() const;
signals:
    void customerAdded(Customer*);
    void vehicleAdded(Vehicle*);
    void customerDestroyed(Agent*);
    void vehicleDestroyed(Agent*);
    void customerTaken(Customer*);
public slots:
};

#endif // WORLD_H
