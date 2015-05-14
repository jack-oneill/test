#include "customergenerator.h"
#include <QTextStream>
#include "world.h"
#include <QFile>
#include <cmath>

CustomerGenerator* CustomerGenerator::myInstance=NULL;
CustomerGenerator* CustomerGenerator::instance(World* wrld,double seed)
{
   if(myInstance==NULL)
   {
       myInstance=new CustomerGenerator(wrld,seed);
   }
   return myInstance;
}

CustomerGenerator::CustomerGenerator(World* world, double seed)
{
    myWorld=world;
    myIndexMap=NULL;
    myGenerator=new boost::random::mt19937(seed);
    myUniform=new boost::random::uniform_01<boost::random::mt19937>(*myGenerator);
}
CustomerGenerator::~CustomerGenerator()
{
    if(myGenerator)
        delete myGenerator;
    if(myUniform)
        delete myUniform;
    if(myIndexMap)
        delete myIndexMap;
}

void CustomerGenerator::setSeed(double seed)
{
    if(myGenerator)
        delete myGenerator;
    if(myUniform)
        delete myUniform;
    myGenerator=new boost::random::mt19937(seed);
    myUniform=new boost::random::uniform_01<boost::random::mt19937>(*myGenerator);
}

QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node> CustomerGenerator::choosePivots()
{
    QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node>  pivots;
    double randval=(*myUniform)();
    int row=0;
    int col=0;
    for(int i=0;i<myRowCdf.size();++i)
    {
        if(randval<=myRowCdf[i])
        {
            row=i;
            break;
        }
    }
    double sum=0;
    if(row!=0)
        sum=myRowCdf[row-1];
    for(int i=0;i<myMatrix[row].size();++i)
    {
       sum+=myMatrix[row][i];
       if(randval<=sum)
       {
            col = i;
            break;
       }
    }
    pivots.first=(*myIndexMap)((unsigned)row);
    pivots.second=(*myIndexMap)((unsigned)col);
    return pivots;
}

lemon::SmartDigraph::Node CustomerGenerator::chooseNode(lemon::SmartDigraph::Node pivot)
{
    QList<lemon::SmartDigraph::Node> nodes = myNeighborhoodMap[pivot];
    QList<double> cdf;
    double val=0;
    QPointF p0= (*myWorld->network()->positionMap())[pivot];
    for(int i=0;i<nodes.size();++i)
    {
        QPointF p1= (*myWorld->network()->positionMap())[nodes[i]];
        QPointF d = p0-p1;
        double dist =sqrt(QPointF::dotProduct(d,d));
        val+= exp(-dist*dist/25);
        if(i!=0)
            cdf.append(val+cdf[i-1]);
        else
            cdf.append(val);
    }
    double randval=(*myUniform)();
    for(int i=0;i<nodes.size();++i)
    {
        if(cdf[i]>=randval*val)
            return nodes[i];
    }
    return lemon::INVALID;
}

bool CustomerGenerator::load2(const QString& name)
{
    if(myWorld==NULL)
        return false;
    QFile file(name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    lemon::SmartDigraph* graph = myWorld->network()->graph();
    if(myIndexMap)
        delete myIndexMap;
    myIndexMap = new IndexMap(*graph);
    myNeighborhoodMap.clear();
    return false;
}

bool CustomerGenerator::load(const QString& name)
{
    if(myWorld==NULL)
        return false;
    QFile file(name);
    QSet<uint64_t> assigned;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    lemon::SmartDigraph* graph = myWorld->network()->graph();
    if(myIndexMap)
        delete myIndexMap;
    myIndexMap = new IndexMap(*graph);
    myNeighborhoodMap.clear();
    myMatrix.clear();
    QTextStream txt(&file);
    QString line;
    unsigned val=0;
    if(txt.atEnd())
        return false;
    line = txt.readLine();
    unsigned num = line.split(",")[0].toUInt();
    myMatrix.resize(num);
    for(unsigned i=0;i<num;++i)
    {
        myMatrix[i].resize(num);
        for(int j=0;j<num;++j)
            myMatrix[i][j]=0;
    }
    lemon::SmartDigraph::Node node;
    double totalWeight=0;
    while(!txt.atEnd())
    {
        line=txt.readLine();
        QStringList lline=line.split(",");
        if(lline.size()!=3)
            continue;
        uint64_t srcId = lline[0].toULongLong();
        uint64_t tgtId = lline[1].toULongLong();
        unsigned posSrc=0;
        unsigned posTgt=0;
        if(!assigned.contains(srcId))
        {
            assigned.insert(srcId);
            node = (*myWorld->network()->nodeIdMap())(srcId);
            myIndexMap->set(node,val);
            posSrc=val;
            val++;
        }
        else
        {
            node = (*myWorld->network()->nodeIdMap())(srcId);
            posSrc =(*myIndexMap)[node];
        }
        if(!assigned.contains(tgtId))
        {
            assigned.insert(tgtId);
            node = (*myWorld->network()->nodeIdMap())(tgtId);
            myIndexMap->set(node,val);
            posTgt=val;
            val++;
        }
        else
        {
            node = (*myWorld->network()->nodeIdMap())(tgtId);
            posTgt =(*myIndexMap)[node];
        }
        double prob  = lline[2].toDouble();
        totalWeight+=prob;
        myMatrix[posSrc][posTgt]=prob;
    }
    for(int i=0;i<num;++i)
    {
        for(int j=0;j<num;++j)
            myMatrix[i][j]/=totalWeight;
    }
    for(lemon::SmartDigraph::NodeIt nit(*graph);nit!=lemon::INVALID;++nit)
    {
        for(QSet<uint64_t>::iterator it = assigned.begin();it!=assigned.end();++it)
        {
            lemon::SmartDigraph::Node node = (*myWorld->network()->nodeIdMap())(*it);
            QPointF posA = (*myWorld->network()->positionMap())[node];
            QPointF posB = (*myWorld->network()->positionMap())[nit];
            QPointF d = posA-posB;
            if(sqrt(QPointF::dotProduct(d,d))<1.0)
                myNeighborhoodMap[node].append(nit);
        }
    }
    myRowCdf.resize(num);
    for(int i=0;i<num;++i)
    {
       double rval = 0;
       for(int j=0;j<num;j++)
       {
            rval+=myMatrix[i][j];
       }
       if(i!=0)
           myRowCdf[i]=rval+myRowCdf[i-1];
       else
           myRowCdf[i]=rval;
    }
    return true;
}

QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node> CustomerGenerator::generate()
{
    QPair<lemon::SmartDigraph::Node,lemon::SmartDigraph::Node>  pivots = choosePivots();
    pivots.first=chooseNode(pivots.first);
    pivots.second=chooseNode(pivots.second);
    return pivots;
}
