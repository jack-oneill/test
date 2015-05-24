#include "customerview.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPainterPath>

CustomerView::CustomerView(Customer* customer, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    myCustomer=customer;
    //setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setToolTip(myCustomer->name());
    setZValue(1);
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void CustomerView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    QBrush obrush = painter->brush();
    painter->setBrush(Qt::green);
    painter->drawRect(QRectF(QPointF(-1,-1),QSizeF(2,2)));
    painter->setBrush(obrush);
}

QPainterPath CustomerView::shape() const
{
    QPainterPath path;
    QRectF rect(-1,-1,2,2);
    path.addRect(rect);
    return path;
}

QRectF CustomerView::boundingRect() const
{
    QRectF rect(-1,-1,2,2);
    return rect;
}
