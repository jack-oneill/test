#ifndef SIMGRAPHICSVIEW_H
#define SIMGRAPHICSVIEW_H
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHash>
#include "agent.h"
#include "world.h"
class QTimer;
class SimGraphicsView : public QGraphicsView
{
    Q_OBJECT
    World* myWorld;
    QGraphicsScene* myScene;
    QHash<Agent*,QGraphicsItem*> myItemMap;
    double myZoomValue;
    void setupMatrix();
    qreal totalScaleFactor;
    QTimer* myRefreshTimer;
    QLabel* myTimeLabel;
public:
    explicit SimGraphicsView(QLabel* = 0,QWidget *parent = 0);
    void setWorld(World*);
    World* world();
protected:
    void wheelEvent(QWheelEvent *event);
    bool viewportEvent(QEvent *event);
signals:

public slots:
    void refresh();
    void addVehicle(Vehicle*);
    void addCustomer(Customer*);
    void removeAgent(Agent*);
    void hideCustomer(Customer*);
    void startRefreshCycle();
    void stopRefreshCycle();
};

#endif // SIMGRAPHICSVIEW_H
