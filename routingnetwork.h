#ifndef ROUTINGNETWORK_H
#define ROUTINGNETWORK_H
#include <lemon/concepts/digraph.h>
#include <lemon/core.h>
#include <lemon/maps.h>
#include <lemon/smart_graph.h>
#include <QPointF>
#include <QStringList>
#include <QVector>
class QFile;

bool operator<(QPointF a,QPointF b);
class Neighborhood
{
private:
    QPointF myCenter;
    QString myName;
    QVector<QPair<lemon::SmartDigraph::Node,double> > myNodes;
    double myRadius;
public:
    Neighborhood(const QString &, const QPointF&, const double );
    ~Neighborhood();
    QString name() const;
    void addNode(const lemon::SmartDigraph::Node&,double&);
    QVector<QPair<lemon::SmartDigraph::Node,double> > nodes() const;
    double radius() const;
    QPointF center() const;
    void toCdf();
    lemon::SmartDigraph::Node get(const double&) const;

};

class RoutingNetwork
{
public:
    typedef lemon::CrossRefMap<lemon::SmartDigraph,lemon::SmartDigraph::Node, unsigned long long> NodeIdMap;
    typedef lemon::CrossRefMap<lemon::SmartDigraph,lemon::SmartDigraph::Arc,unsigned long long> ArcIdMap;
    typedef lemon::IterableValueMap<lemon::SmartDigraph,lemon::SmartDigraph::Arc,double> DistanceMap;
    typedef lemon::IterableValueMap<lemon::SmartDigraph,lemon::SmartDigraph::Node,QPointF> PositionMap;
protected:
    QVector<Neighborhood*> myNeighborhoods;
    lemon::SmartDigraph* myGraph;
    NodeIdMap* myNodeIdMap;
    ArcIdMap* myArcIdMap;
    DistanceMap* myDistanceMap;
    PositionMap* myPositionMap;
    void initialize();
    void addNode(const QStringList&);
    void addArc(const QStringList&);
    QString myCurrentNetwork;
    QPointF myCenter;
    bool loadNeighborhoods(QFile &);
    bool myValid;
public:
    RoutingNetwork(const QString&);
    ~RoutingNetwork();
    bool load(const QString&);
    QString loaded() const;
    lemon::SmartDigraph* graph() const;
    NodeIdMap* nodeIdMap() const;
    ArcIdMap* arcIdMap() const;
    DistanceMap* distanceMap() const;
    PositionMap* positionMap() const;
    QPointF center() const;
    QVector<Neighborhood*> neigborhoods() const;
    bool valid() const;
};

#endif // ROUTINGNETWORK_H
