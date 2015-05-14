#ifndef STREETLINE_H
#define STREETLINE_H

#include <QGraphicsLineItem>

class StreetLine : public QGraphicsLineItem
{
public:
    StreetLine(QGraphicsItem *parent = 0);
    StreetLine(const QLineF&, QGraphicsItem* parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:

};

#endif // STREETLINE_H
