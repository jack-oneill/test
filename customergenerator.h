#ifndef CUSTOMERGENERATOR_H
#define CUSTOMERGENERATOR_H
#include <lemon/core.h>
#include <lemon/maps.h>
#include <lemon/smart_graph.h>
#include <QVector>
#include <QPair>
#include <QMap>
#include <boost/random.hpp>
class World;
class EventShowUp;
class CustomerGenerator
{
public:
    typedef lemon::CrossRefMap<lemon::SmartDigraph,lemon::SmartDigraph::Node,unsigned> IndexMap;
    typedef QMap<lemon::SmartDigraph::Node,QList<lemon::SmartDigraph::Node> > NeighborhoodMap;

protected:
    World* myWorld;
    boost::random::mt19937* myGenerator;
    boost::random::uniform_01<boost::random::mt19937>* myUniform;
    QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node> choosePivots();
    lemon::SmartDigraph::Node chooseNode(lemon::SmartDigraph::Node);
    QVector<QVector<double> > myMatrix;
    QVector<double > myRowCdf;
    IndexMap* myIndexMap;
    NeighborhoodMap myNeighborhoodMap;

    QVector<double> myNeighborhoods;
    QVector<QVector<QPair<lemon::SmartDigraph::Node,double> > > myNodes;
    static CustomerGenerator* myInstance;
    CustomerGenerator(World*, double seed=0);
    ~CustomerGenerator();
public:
    static CustomerGenerator* instance(World*,double seed=0);
    void setSeed(double);
    bool load(const QString&);
    bool load2(const QString&);
    QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node> generate();
};

#endif // CUSTOMERGENERATOR_H
