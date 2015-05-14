#ifndef CUSTOMERGENERATOROFFLINE_H
#define CUSTOMERGENERATOROFFLINE_H
#include <QString>
#include <QVector>
#include <lemon/smart_graph.h>
class Customer;
class World;
class CustomerGeneratorSample
{
private:
    unsigned mySrc;
    unsigned myTgt;
    uint64_t myStart;
    uint64_t myReturn;
public:
    CustomerGeneratorSample(const unsigned& src, const unsigned& tgt,uint64_t&,uint64_t&);
    unsigned source();
    unsigned target();
    uint64_t start();
    uint64_t ret();
};

class CustomerGeneratorOffline
{
private:
    double mySeed;
    int myNoise;
    World* myWorld;
    QVector<CustomerGeneratorSample*> mySamples;
    void clear();
public:
    CustomerGeneratorOffline(World*, const QString&, const int noise=0, const double=0);
    ~CustomerGeneratorOffline();
    void setSeed(const double&);
    bool load(const QString&);
    void generate(const unsigned &num);
    QVector<lemon::SmartDigraph::Node> disperseVehicles(const unsigned& num);
};

#endif // CUSTOMERGENERATOROFFLINE_H
