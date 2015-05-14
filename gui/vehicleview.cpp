#include "vehicleview.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPainterPath>

VehicleView::VehicleView(Vehicle *vehicle, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    myVehicle=vehicle;
    setFlag(ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

void VehicleView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush obrush = painter->brush();
    painter->setBrush(Qt::red);
    painter->drawEllipse(pos(),8,8);
    painter->setBrush(obrush);
}

QPainterPath VehicleView::shape() const
{
    QPainterPath path;
    QRectF rect(-4,-4,4,4);
    path.addEllipse(rect);
    return path;
}

QRectF VehicleView::boundingRect() const
{
    QRectF rect(-5,-5,5,5);
    return rect;
}
