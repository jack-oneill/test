#include "customerview.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPainterPath>

CustomerView::CustomerView(Customer* customer, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    myCustomer=customer;
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

void CustomerView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QBrush obrush = painter->brush();
    painter->setBrush(Qt::green);
    painter->drawRect(QRectF(pos(),QSizeF(8,8)));
    painter->setBrush(obrush);
}

QPainterPath CustomerView::shape() const
{
    QPainterPath path;
    QRectF rect(-4,-4,4,4);
    path.addEllipse(rect);
    return path;
}

QRectF CustomerView::boundingRect() const
{
    QRectF rect(-5,-5,5,5);
    return rect;
}
