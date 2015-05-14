#ifndef VEHICLEVIEW_H
#define VEHICLEVIEW_H

#include <QGraphicsItem>
#include "vehicle.h"
class VehicleView : public QGraphicsItem
{
    Vehicle* myVehicle;
public:
    VehicleView(Vehicle*,QGraphicsItem*parent = 0);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    QRectF boundingRect() const;
};

#endif // VEHICLEVIEW_H
