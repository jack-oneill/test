#include "streetline.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>
StreetLine::StreetLine(QGraphicsItem *parent ) : QGraphicsLineItem(parent)
{
    //setCacheMode(QGraphicsItem::ItemCoordinateCache);
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}
StreetLine::StreetLine(const QLineF &line, QGraphicsItem* parent ) : QGraphicsLineItem(line,parent)
{
    //setCacheMode(QGraphicsItem::ItemCoordinateCache);
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void StreetLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    if(option->levelOfDetailFromTransform(painter->worldTransform())<3 && line().length()<1)
        return;
    if(option->levelOfDetailFromTransform(painter->worldTransform())<2 && line().length()<0.5)
        return;
    QGraphicsLineItem::paint(painter,option,widget);
}
