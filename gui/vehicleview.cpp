#include "vehicleview.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPainterPath>

VehicleView::VehicleView(Vehicle *vehicle, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    myVehicle=vehicle;
    setFlag(ItemIsSelectable);
    setZValue(1);
    //setFlag(QGraphicsItem::ItemIgnoresTransformations);
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void VehicleView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    QBrush obrush = painter->brush();
    painter->setBrush(Qt::red);
    painter->drawEllipse(-1,-1,2,2);
    painter->setBrush(obrush);
}

QPainterPath VehicleView::shape() const
{
    QPainterPath path;
    QRectF rect(-1,-1,2,2);
    path.addEllipse(rect);
    return path;
}

QRectF VehicleView::boundingRect() const
{
    QRectF rect(-1,-1,2,2);
    return rect;
}
