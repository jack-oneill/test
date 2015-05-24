#include "simgraphicsview.h"
#include <QMutexLocker>
#include <QDebug>
#include "vehicleview.h"
#include "customerview.h"
#include "../simulationkernel.h"
#include <QPen>
#include <QNativeGestureEvent>
#include <qmath.h>
#include <QWheelEvent>
#include <QTimer>
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif
#include <QGraphicsItemGroup>
#include "streetline.h"
#define MY_FACTOR 1000
SimGraphicsView::SimGraphicsView(QLabel* lab, QWidget *parent) :
    QGraphicsView(parent)
{
    myScene=NULL;
    myZoomValue=250;
    myTimeLabel=lab;
    totalScaleFactor=1;
    setDragMode(ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing, false);
    setOptimizationFlags(QGraphicsView::DontSavePainterState );
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //setCacheMode(QGraphicsView::CacheBackground);
    //setViewport( new QGLWidget(QGLFormat(QGL::AccumBuffer | QGL::SampleBuffers | QGL::SingleBuffer | QGL::DirectRendering)));
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    myRefreshTimer=new QTimer(this);
    connect(myRefreshTimer,SIGNAL(timeout()),this,SLOT(refresh()));
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
    QPen pen(QColor("blue"),0);
    QPointF offset=myWorld->network()->center();
    uint64_t skip=0;
    double pixPerM=0;
    lemon::SmartDigraph::ArcIt testit(*graph);
    double km =(*net->distanceMap())[testit];
    posA=(*net->positionMap())[graph->source(testit)]-offset;
    posB=(*net->positionMap())[graph->target(testit)]-offset;
    posA.setY(-1*posA.y());
    posB.setY(-1*posB.y());
    posA=MY_FACTOR*posA;
    posB=MY_FACTOR*posB;
    pixPerM=qSqrt(QPointF::dotProduct(posA-posB,posA-posB))/(km*1000);
    QPointF mid;
    for(lemon::SmartDigraph::ArcIt ait(*graph);ait!=lemon::INVALID;++ait)
    {
       skip++;
       if(skip%2==0)
           continue;
       a=graph->source(ait);
       b=graph->target(ait);
       posA=(*net->positionMap())[a]-offset;
       posB=(*net->positionMap())[b]-offset;
       posA.setY(-1*posA.y());
       posB.setY(-1*posB.y());
       line.setP1(posA*MY_FACTOR);
       line.setP2(posB*MY_FACTOR);
       mid=0.5*line.p1()+0.5*line.p2();
       line.setP1(line.p1()-mid);
       line.setP2(line.p2()-mid);
       StreetLine* sline = new StreetLine(line);
       sline->setToolTip(QString::number((*net->positionMap())[a].x())+", "+QString::number((*net->positionMap())[a].y())
       +" to "+QString::number((*net->positionMap())[b].x())+", "+QString::number((*net->positionMap())[b].y()));
       //sline->setZValue(-1);
       sline->setPen(pen);
       sline->setPos(mid);
       myScene->addItem(sline);

    }
    qDebug()<<"Graphics scene size : "+QString::number(skip/2);
    qDebug()<<"Pixels per KM : " + QString::number(pixPerM);
    QGraphicsEllipseItem* zeroZero= new QGraphicsEllipseItem(NULL);
    zeroZero->setRect(-2.5,-2.5,5,5);
    zeroZero->setBrush(Qt::green);
    QPen pen2 = zeroZero->pen();
    pen2.setWidth(0);
    zeroZero->setPen(pen2);
    myScene->addItem(zeroZero);
    foreach(Neighborhood* neigh,net->neigborhoods())
    {
        QGraphicsEllipseItem* item = new QGraphicsEllipseItem(NULL);
        double dwidth = neigh->radius()*pixPerM;
        item->setRect((neigh->center().x()-offset.x())*MY_FACTOR-dwidth,(neigh->center().y()-offset.y())*-MY_FACTOR-dwidth,2*dwidth,2*dwidth);
        item->setBrush(Qt::gray);
        item->setOpacity(.3);
        item->setToolTip(neigh->name());
        item->setPen(pen);
        myScene->addItem(item);

        //TEST DIST
        /*for(int i=0;i<50;++i)
        {
            lemon::SmartDigraph::Node nod = neigh->get((rand()%100)/100.0);
            QPointF pf = (*net->positionMap())[nod];
            pf=pf-offset;
            pf=pf*MY_FACTOR;
            pf.setY(pf.y()*-1);
            item = new QGraphicsEllipseItem(NULL);
            item->setRect(pf.x()-0.5,pf.y()-0.5,1,1);
            item->setBrush(Qt::red);
            myScene->addItem(item);

        }*/

    }

    pen.setColor(Qt::red);
    connect(myWorld,SIGNAL(customerAdded(Customer*)),this,SLOT(addCustomer(Customer*)));
    connect(myWorld,SIGNAL(vehicleAdded(Vehicle*)),this,SLOT(addVehicle(Vehicle*)));
    connect(myWorld,SIGNAL(vehicleDestroyed(Agent*)),this,SLOT(removeAgent(Agent*)));
    connect(myWorld,SIGNAL(customerDestroyed(Agent*)),this,SLOT(removeAgent(Agent*)));
    connect(myWorld,SIGNAL(customerTaken(Customer*)),this,SLOT(hideCustomer(Customer*)));
    //myScene->addEllipse(QPointF(),20,20);
}

World* SimGraphicsView::world()
{
    return myWorld;
}

void SimGraphicsView::refresh()
{
    QMutexLocker locker(&myWorld->kernel()->viewRefreshMutex());
    if(myWorld->vehicles().size()==0)
        return;
    QMainWindow* mainWindow=(QMainWindow*)parent();
    QHash<Agent*,QGraphicsItem*>::iterator it= myItemMap.begin();
    for(;it!=myItemMap.end();++it)
    {
        if(it.key()->type()==VEHICLE)
        {
            Vehicle* vehicle = (Vehicle*) it.key();
            double covered =  vehicle->position(myWorld->time()).second;
            if(covered>1)
                covered=1;
            QPointF p1 = (*myWorld->network()->positionMap())[ vehicle->position(myWorld->time()).first]-myWorld->network()->center();
            QPointF p2 = (*myWorld->network()->positionMap())[vehicle->nextPosition()]-myWorld->network()->center();
            p1*=MY_FACTOR;
            p2*=MY_FACTOR;
            p1.setY(-1*p1.y());
            p2.setY(-1*p2.y());
            if(p1==p2)
                it.value()->setPos(p1);
            else
                it.value()->setPos(covered*p2+(1-covered)*p1);
        }
    }
    myScene->update();
    uint64_t time = myWorld->time();
    uint64_t hours = time/3600;
    uint64_t minutes = time/60-hours*60;
    uint64_t seconds = time-hours*3600-minutes*60;
    if(myTimeLabel)
        myTimeLabel->setText(QString::number(hours)+":"+QString::number(minutes)+":"+QString::number(seconds));
}

void SimGraphicsView::addVehicle(Vehicle* veh)
{
    QGraphicsItem* item =new VehicleView(veh);
    myItemMap[(Agent*)veh]=item;
    //item->setZValue(1);
    QPointF pos =(*myWorld->network()->positionMap())[veh->position(myWorld->time()).first];
    pos-=myWorld->network()->center();
    pos.setX(pos.x()*MY_FACTOR);
    pos.setY(pos.y()*-MY_FACTOR);
    item->setPos(pos);
    myScene->addItem(item);
}

void SimGraphicsView::addCustomer(Customer* cust)
{
    QMutexLocker lock(&myWorld->kernel()->viewRefreshMutex());
    if(!myWorld->customers().contains(cust))
        return;
    QGraphicsItem* item =new CustomerView(cust);
    myItemMap[(Agent*)cust]=item;
    QPointF pos =(*myWorld->network()->positionMap())[cust->position(myWorld->time()).first];
    pos-=myWorld->network()->center();
    pos*=MY_FACTOR;
    pos.setY(pos.y()*-1);
    item->setPos(pos);
    myScene->addItem(item);

}

void SimGraphicsView::removeAgent(Agent* agent)
{
    QMutexLocker lock(&myWorld->kernel()->viewRefreshMutex());
    QHash<Agent*,QGraphicsItem*>::iterator it= myItemMap.find(agent);
    if(it!=myItemMap.end())
    {
        myScene->removeItem(it.value());
        myItemMap.erase(it);
        //delete it.value();
    }
}

void SimGraphicsView::hideCustomer(Customer* cust)
{
    QHash<Agent*,QGraphicsItem*>::iterator it= myItemMap.find(cust);
    if(it!=myItemMap.end())
        it.value()->setVisible(!it.value()->isVisible());
    myScene->update();
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
         //       qDebug()<<myZoomValue;
                return true;
            }
        }
        break;
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}

void SimGraphicsView::startRefreshCycle()
{
    myRefreshTimer->setSingleShot(false);
    myRefreshTimer->start(1000);
}

void SimGraphicsView::stopRefreshCycle()
{
    myRefreshTimer->stop();
}
