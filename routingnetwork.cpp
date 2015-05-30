#include "routingnetwork.h"
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <cmath>
#include <QDebug>
#include "utilities.h"

bool operator<(QPointF a,QPointF b)
{
    return  (a.x()+a.y())<(b.x()+b.y());
}

Neighborhood::Neighborhood(const QString& name, const QPointF& center, const double rad)
{
    myRadius=rad;
    myCenter=center;
    myName=name;
    myCentralNode = lemon::INVALID;
}
Neighborhood::~Neighborhood()
{

}

QString Neighborhood::name() const
{
    return myName;
}
void Neighborhood::addNode(const lemon::SmartDigraph::Node& node,double& weight)
{
    myNodes.append(QPair<lemon::SmartDigraph::Node,double>(node,weight));
}

void Neighborhood::setCentralNode(const lemon::SmartDigraph::Node& nod)
{
   myCentralNode=nod;
}

QVector<QPair<lemon::SmartDigraph::Node,double> > Neighborhood::nodes() const
{
    return myNodes;
}
double Neighborhood::radius() const
{
    return myRadius;
}
QPointF Neighborhood::center() const
{
    return myCenter;
}
bool Neighborhood::contains(const lemon::SmartDigraph::Node& nod) const
{
    //Linear search ... stupid
    for(int i=0;i<myNodes.size();++i)
    {
        if(myNodes[i].first==nod)
            return true;
    }
    return false;
}

void Neighborhood::toCdf()
{
    double acc=0;
    for(int i=0;i<myNodes.size();++i)
    {
        myNodes[i].second+=acc;
        acc=myNodes[i].second;
    }
    for(int i=0;i<myNodes.size();++i)
        myNodes[i].second=myNodes[i].second/acc;
}
lemon::SmartDigraph::Node Neighborhood::get(const double& pval) const
{
    for(int i=0;i<myNodes.size();++i)
    {
        if(myNodes[i].second>=pval)
            return myNodes[i].first;
    }
    return myNodes[myNodes.size()-1].first;
}

lemon::SmartDigraph::Node Neighborhood::centralNode() const
{
    return myCentralNode;
}

RoutingNetwork::RoutingNetwork(const QString& file)
{
    myGraph=NULL;
    myArcIdMap=NULL;
    myNodeIdMap=NULL;
    myDistanceMap=NULL;
    myPositionMap=NULL;
    myCenter.setX(0);
    myCenter.setY(0);
    myMaxNeighborhoodRadius=0;
    myValid=load(file);
}

RoutingNetwork::~RoutingNetwork()
{
    if(myGraph)
        delete myGraph;
    if(myArcIdMap)
        delete myArcIdMap;
    if(myNodeIdMap)
        delete myNodeIdMap;
    if(myDistanceMap)
        delete myDistanceMap;
    if(myPositionMap)
        delete myPositionMap;
    for(int i=0;i<myNeighborhoods.size();++i)
    {
        delete myNeighborhoods[i];
    }
}

void RoutingNetwork::addNode(const QStringList& desc)
{
    unsigned long long id = desc[0].toULongLong();
    QPointF pos;
    pos.setX(desc[1].toDouble());
    pos.setY(desc[2].toDouble());
    lemon::SmartDigraph::Node node=myGraph->addNode();
    myNodeIdMap->set(node,id);
    myPositionMap->set(node,pos);
    myCenter+=pos;
}

void RoutingNetwork::addArc(const QStringList& desc)
{
    unsigned long long id = desc[0].toULongLong();
    unsigned long long idS = desc[1].toULongLong();
    unsigned long long idT = desc[2].toULongLong();
    double dist = desc[7].toDouble();
    lemon::SmartDigraph::Node s,t;
    s=(*myNodeIdMap)(idS);
    t=(*myNodeIdMap)(idT);
    if(myGraph->id(s)<0 || myGraph->id(t)<0)
        return;
    lemon::SmartDigraph::Arc arc=myGraph->addArc(s,t);
    myArcIdMap->set(arc,id);
    QPointF diff=(*myPositionMap)[s]-(*myPositionMap)[t];
    myDistanceMap->set(arc,dist);
}

void RoutingNetwork::initialize()
{

    if(myGraph)
        delete myGraph;
    if(myArcIdMap)
        delete myArcIdMap;
    if(myNodeIdMap)
        delete myNodeIdMap;
    if(myPositionMap)
        delete myPositionMap;
    if(myDistanceMap)
        delete myDistanceMap;
    myGraph=new lemon::SmartDigraph();
    myNodeIdMap=new NodeIdMap(*myGraph);
    myArcIdMap=new ArcIdMap(*myGraph);
    myDistanceMap=new DistanceMap(*myGraph);
    myPositionMap=new PositionMap(*myGraph);
}

bool RoutingNetwork::loadNeighborhoods(QFile &file)
{
    QTextStream txt(&file);
    QString line;
    QString name;
    double lattitude=0;
    double longtitude=0;
    QPointF point;
    double radius=0;
    bool isDouble=true;
    bool isInt=true;
    while(!txt.atEnd())
    {
        line=txt.readLine();
        QStringList lline=line.split(",");
        lline[1].toInt(&isInt);
        if(lline.length()<4)
        {
            qDebug()<<"Wrong neighborhood file!";
            file.close();
            return false;
        }
        name = lline[0];
        lattitude=lline[1].toDouble(&isDouble);
        longtitude=lline[2].toDouble(&isDouble);
        radius=lline[3].toDouble(&isDouble);
        if(!isDouble)
        {
            qDebug()<<"Wrong neighborhood file entry! Expected a double precision value.";
            return false;
        }
        myNeighborhoods.append(new Neighborhood(name,QPointF(longtitude,lattitude),radius));
    }
    QList<Neighborhood*> nList;
    for(lemon::SmartDigraph::NodeIt nit(*myGraph);nit!=lemon::INVALID;++nit)
    {
        for(int i=0;i<myNeighborhoods.size();++i)
        {
            QPointF np=(*myPositionMap)[nit];
            Neighborhood* neigh = myNeighborhoods[i];
            double di = Utilities::dist(np.y(),np.x(),neigh->center().y(),neigh->center().x());
            if(di<=neigh->radius())
            {
                nList.append(neigh);
            }
        }
        for(int i=0;i<nList.size();++i)
        {
           double val = 1.0/nList.size();
           nList[i]->addNode(nit,val);
        }
        nList.clear();
    }
    for(int i=0;i<myNeighborhoods.size();++i)
    {
        myNeighborhoods[i]->toCdf();
        myMaxNeighborhoodRadius=fmax(myNeighborhoods[i]->radius(),myMaxNeighborhoodRadius);
        QVector<QPair<lemon::SmartDigraph::Node,double> > nodes = myNeighborhoods[i]->nodes();
        double minDist=6000;
        Neighborhood* neigh = myNeighborhoods[i];
        for(int j=0;j<nodes.size();++j)
        {
            QPointF np=(*myPositionMap)[nodes[j].first];
            double di = Utilities::dist(np.y(),np.x(),neigh->center().y(),neigh->center().x());
            if(di<minDist)
            {
                minDist=di;
                neigh->setCentralNode(nodes[j].first);
            }

        }
        qDebug()<<"Neighborhood "+QString::number(i)+" has "+QString::number(myNeighborhoods[i]->nodes().size())+" nodes ";
    }
    return true;
}

bool RoutingNetwork::load(const QString& name)
{
    QFile file(name);
    QFile hoods(name.mid(0,name.lastIndexOf("."))+".neigh");
    bool readingNodes=true;
    myCurrentNetwork="None";
    if(!file.open(QFile::ReadOnly))
    {
        qDebug()<<"Couldn't open network";
        return false;
    }
    if(!hoods.open(QFile::ReadOnly))
    {
        qDebug()<<"Couldn't open neighborhoods";
        return false;
    }
    myCurrentNetwork=name;
    QTextStream txt(&file);
    QString line;
    initialize();
    uint64_t numNodes=0,numArcs=0;
    while(!txt.atEnd())
    {
        line=txt.readLine();
        QStringList lline=line.split(",");
        if(readingNodes && lline[3]!="0")
            readingNodes=false;
        if(readingNodes)
        {
            addNode(lline);
            numNodes++;
        }
        else
        {
            addArc(lline);
            numArcs++;
        }
    }
    myCenter/=numNodes;
    qDebug()<<"Read nodes: "<<numNodes;
    qDebug()<<"Read arcs: "<<numArcs;
    qDebug()<<"Center : "<<myCenter;
    return loadNeighborhoods(hoods);


}

QString RoutingNetwork::loaded() const
{
   return myCurrentNetwork;
}

lemon::SmartDigraph* RoutingNetwork::graph() const
{
    return myGraph;
}
RoutingNetwork::NodeIdMap* RoutingNetwork::nodeIdMap() const
{
    return myNodeIdMap;
}

RoutingNetwork::ArcIdMap* RoutingNetwork::arcIdMap() const
{
    return myArcIdMap;
}
RoutingNetwork::DistanceMap* RoutingNetwork::distanceMap() const
{
    return myDistanceMap;
}
RoutingNetwork::PositionMap* RoutingNetwork::positionMap() const
{
    return myPositionMap;
}
QPointF RoutingNetwork::center() const
{

    return myCenter;
}
QVector<Neighborhood*> RoutingNetwork::neigborhoods() const
{
    return myNeighborhoods;
}
bool RoutingNetwork::valid() const
{
    return myValid;
}

double RoutingNetwork::getMaxNeighborhoodRadius() const
{
    return myMaxNeighborhoodRadius;
}
