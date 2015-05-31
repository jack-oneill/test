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
    if(myVehicle->customers().size()==0)
        painter->setBrush(Qt::yellow);
    else
        painter->setBrush(Qt::red);
    painter->drawEllipse(-1,-1,2,2);
    painter->setBrush(Qt::black);
    QPen pen = painter->pen();
    double width =pen.widthF();
    pen.setWidthF(0.001);
    QColor col = pen.color();
    pen.setColor(Qt::black);
    QFont font = painter->font();
    font.setPointSizeF(1.5);
    painter->setFont(font);
    painter->drawText(QRectF(-1,-1,2,2),Qt::AlignCenter,QString::number(myVehicle->customers().size()));
    pen.setWidth(width);
    pen.setColor(col);
    painter->setPen(pen);
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
