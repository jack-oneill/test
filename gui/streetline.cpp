#include "streetline.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
StreetLine::StreetLine(QGraphicsItem *parent ) : QGraphicsLineItem(parent)
{
 //   setCacheMode(QGraphicsItem::ItemCoordinateCache);
}
StreetLine::StreetLine(const QLineF &line, QGraphicsItem* parent ) : QGraphicsLineItem(line,parent)
{
//    setCacheMode(QGraphicsItem::ItemCoordinateCache);
}

void StreetLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(option->levelOfDetailFromTransform(painter->worldTransform())*line().length()<3)
        return;
    //painter->drawEllipse(line().p1(),2,2);
    //painter->drawEllipse(line().p2(),2,2);
    QGraphicsLineItem::paint(painter,option,widget);
}
