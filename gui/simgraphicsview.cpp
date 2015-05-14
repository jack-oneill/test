#include "simgraphicsview.h"
#include <QDebug>
#include "vehicleview.h"
#include "customerview.h"
#include <QPen>
#include <QNativeGestureEvent>
#include <qmath.h>
#include <QWheelEvent>
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif
#include <QGraphicsItemGroup>
#include "streetline.h"
SimGraphicsView::SimGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    myScene=NULL;
    myZoomValue=250;
    totalScaleFactor=1;
    setDragMode(ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing, false);
    setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing |QGraphicsView::DontSavePainterState );
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setCacheMode(QGraphicsView::CacheBackground);
    //setViewport( new QGLWidget(QGLFormat(QGL::AccumBuffer | QGL::SampleBuffers | QGL::SingleBuffer | QGL::DirectRendering)));
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
}

void SimGraphicsView::setWorld(World* wrld)
{
    if(myScene)
        delete myScene;
    myScene=new QGraphicsScene(this);
    setScene(myScene);
    myWorld=wrld;
    if(wrld==NULL)
        return;
    RoutingNetwork* net = myWorld->network();
    lemon::SmartDigraph* graph=net->graph();
    lemon::SmartDigraph::Node a,b;
    QPointF posA,posB;
    QLineF line;
    QPen pen(QColor("blue"),0.2);
    QPointF offset=myWorld->network()->center();
    uint64_t skip=0;
    for(lemon::SmartDigraph::ArcIt ait(*graph);ait!=lemon::INVALID;++ait)
    {
       skip++;
       if(skip%2==0)
           continue;
       a=graph->source(ait);
       b=graph->target(ait);
       posA=(*net->positionMap())[a]+offset;
       posB=(*net->positionMap())[b]+offset;
       posA.setY(-1*posA.y());
       posB.setY(-1*posB.y());
       line.setP1(posA*1000);
       line.setP2(posB*1000);
       //qDebug()<<line;
       StreetLine* sline = new StreetLine(line);
       sline->setZValue(-1);
       sline->setPen(pen);
       myScene->addItem(sline);
    }
    pen.setColor(Qt::red);
    connect(myWorld,SIGNAL(customerAdded(Customer*)),this,SLOT(addCustomer(Customer*)));
    connect(myWorld,SIGNAL(vehicleAdded(Vehicle*)),this,SLOT(addVehicle(Vehicle*)));
    connect(myWorld,SIGNAL(vehicleDestroyed(Vehicle*)),this,SLOT(removeAgent(Agent*)));
    connect(myWorld,SIGNAL(customerDestroyed(Customer*)),this,SLOT(removeAgent(Agent*)));
    //myScene->addEllipse(QPointF(),20,20);
}

World* SimGraphicsView::world()
{
    return myWorld;
}

void SimGraphicsView::refresh()
{
    QHash<Agent*,QGraphicsItem*>::iterator it= myItemMap.begin();
    for(;it!=myItemMap.end();++it)
    {
        World* world = it.key()->world();
        QPointF p = (*world->network()->positionMap())[ it.key()->position(myWorld->time()).first];
        //TODO: cars
        it.value()->setPos(p);
    }
}

void SimGraphicsView::addVehicle(Vehicle* veh)
{
    QGraphicsItem* item =new VehicleView(veh);
    myItemMap[(Agent*)veh]=item;
    myScene->addItem(item);
    item->setZValue(1);
    QPointF pos =(*myWorld->network()->positionMap())[veh->position(myWorld->time()).first];
    pos+=myWorld->network()->center();
    pos.setX(pos.x()*1000);
    pos.setY(pos.y()*-1000);
    item->setPos(pos);
}

void SimGraphicsView::addCustomer(Customer* cust)
{
    QGraphicsItem* item =new CustomerView(cust);
    myItemMap[(Agent*)cust]=item;
    myScene->addItem(item);
    QPointF pos =(*myWorld->network()->positionMap())[cust->position(myWorld->time()).first];
    pos+=myWorld->network()->center();
    pos.setX(pos.x()*1000);
    pos.setY(pos.y()*-1000);
    item->setPos(pos);

}

void SimGraphicsView::removeAgent(Agent* agent)
{
    QHash<Agent*,QGraphicsItem*>::iterator it= myItemMap.find(agent);
    if(it!=myItemMap.end())
    {
        myScene->removeItem(it.value());
        myItemMap.erase(it);
    }
}

void SimGraphicsView::hideCustomer(Customer* cust)
{
    QHash<Agent*,QGraphicsItem*>::iterator it= myItemMap.find(cust);
    if(it!=myItemMap.end())
        it.value()->setVisible(!it.value()->isVisible());
}

void SimGraphicsView::setupMatrix()
{
    qreal scale = qPow(qreal(2), (myZoomValue - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(scale, scale);
    setMatrix(matrix);
}

void SimGraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0)
        {
            myZoomValue+=6;
            if(myZoomValue>2000)
                myZoomValue=2000;
            setupMatrix();
        }
        else
        {
            myZoomValue-=6;
            if(myZoomValue<0)
                myZoomValue=0;
            setupMatrix();
        }
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
    }
}

bool SimGraphicsView::viewportEvent(QEvent *event)
{
    QNativeGestureEvent* natevent;
    switch (event->type()) {
    case QEvent::NativeGesture:
        natevent = static_cast<QNativeGestureEvent*>(event);
        if(natevent->gestureType()==Qt::ZoomNativeGesture)
        {
            /*myZoomValue+=6*natevent->value();
            if(myZoomValue<=0)
                myZoomValue=0;
            if(myZoomValue>2000)
                myZoomValue=2000;
            setupMatrix();*/
            if(natevent->value()!=0)
            {
                if(natevent->value()>0)
                    myZoomValue+=3;//natevent->value()*myZoomValue*0.1;
                else
                    myZoomValue-=3;
                if(myZoomValue<0)
                    myZoomValue=0;
                setupMatrix();
                qDebug()<<myZoomValue;
                return true;
            }
        }
        break;
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}
