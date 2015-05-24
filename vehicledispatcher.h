#ifndef VEHICLEDISPATCHER_H
#define VEHICLEDISPATCHER_H

#include <QObject>
#include <lemon/smart_graph.h>
#include <QList>
#include <QPair>
class VehicleRouter;
class World;
class Vehicle;
class VehicleDispatcher : public QObject
{
    Q_OBJECT
    World* myWorld;
public:
    explicit VehicleDispatcher(World*,QObject *parent = 0);
    QList<QPair<lemon::SmartDigraph::Node,double> > route(Vehicle*);
signals:

public slots:

};

#endif // VEHICLEDISPATCHER_H
