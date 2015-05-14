#ifndef CUSTOMERVIEW_H
#define CUSTOMERVIEW_H

#include <QGraphicsItem>
#include "customer.h"

class CustomerView : public QGraphicsItem
{
private:
    Customer* myCustomer;
public:
    CustomerView(Customer*,QGraphicsItem *parent = 0);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    QRectF boundingRect() const;

};

#endif // CUSTOMERVIEW_H
