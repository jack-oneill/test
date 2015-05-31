#ifndef VEHICLEDISPATCHER_H
#define VEHICLEDISPATCHER_H

#include "customergeneratoroffline.h"
#include <QObject>
#include <QMap>
#include <lemon/smart_graph.h>
#include <QList>
#include <QPair>
#include <QSet>
class VehicleRouter;
class World;
class Vehicle;
class VehicleDispatcher : public QObject
{
    Q_OBJECT
private:
    World* myWorld;
    CustomerGeneratorOffline* myGenerator;
    QMap<uint64_t,QVector<unsigned> > myHistogram;
    QSet<unsigned> myValidNeighborhoods;
    QVector<QList<uint64_t> > myDispatchedVehicles;
    QVector<unsigned> myLocatedVehicles;
    QMap<Vehicle*,QList<unsigned> > myTabuLists;
    double myAverageSeats;
    void constructHistogram();
    int locateVehicles(Vehicle*);
    void prune();
    double score(Vehicle*,const unsigned&);
    explicit VehicleDispatcher();
    static VehicleDispatcher* myInstance;
    unsigned integrate(uint64_t,uint64_t,unsigned);
public:
    void route(Vehicle*);
    void setup(World*,CustomerGeneratorOffline*);
    static VehicleDispatcher* instance();

signals:

public slots:

};

#endif // VEHICLEDISPATCHER_H
