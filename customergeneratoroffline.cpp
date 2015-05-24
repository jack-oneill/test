#include "customergeneratoroffline.h"
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include "world.h"
#include "event.h"
#include <boost/random.hpp>
#include "agentfactory.h"

CustomerGeneratorSample::CustomerGeneratorSample(const unsigned& src, const unsigned& tgt,uint64_t& st,uint64_t& r)
{
    mySrc=src;
    myTgt=tgt;
    myStart=st;
    myReturn=r;
}

unsigned CustomerGeneratorSample::source()
{
   return mySrc;
}

unsigned CustomerGeneratorSample::target()
{
    return myTgt;
}

uint64_t CustomerGeneratorSample::start()
{
    return myStart;
}

uint64_t CustomerGeneratorSample::ret()
{
    return myReturn;
}

CustomerGeneratorOffline::CustomerGeneratorOffline(World* world,const QString& file,const int noise,  const double seed)
{
    myNoise=noise;
    mySeed=seed;
    myWorld=world;
    load(file);
}

CustomerGeneratorOffline::~CustomerGeneratorOffline()
{
    clear();
}
void CustomerGeneratorOffline::clear()
{
    for(int i=0;i<mySamples.size();++i)
        delete mySamples[i];
}

bool CustomerGeneratorOffline::load(const QString &file)
{
    if(file=="")
        return false;
    QFile data(file);
    if(!data.open(QIODevice::ReadOnly))
    {
        qDebug()<<"CustomerGeneratorOffline: Failed to open file "+file;
        return false;
    }
    clear();
    QTextStream txt(&data);
    QString line;
    QStringList tokens;
    bool isDouble=true;
    bool isInt=true;
    unsigned src=0;
    unsigned tgt=0;
    uint64_t dep=0;
    uint64_t ret=0;
    uint64_t lNum=1;
    while(!txt.atEnd())
    {
       line=txt.readLine();
       tokens=line.split(",");
       if(tokens.size()!=3)
       {
           qDebug()<<"CustomerGeneratorOffline: Wrong trip distribution file format!";
           return false;
       }
       src=tokens[0].toInt(&isInt);
       tgt=tokens[1].toInt(&isInt);
       dep=(unsigned)tokens[2].toDouble(&isDouble)*60;
       dep*=60;
       if(!isInt || !isDouble)
       {
          qDebug()<<"CustomerGeneratorOffline: Wrong information in trip distribution file. Could not read line "<<lNum;
          return false;
       }
       mySamples.append(new CustomerGeneratorSample(src,tgt,dep,ret));
    }
    return true;
}

void CustomerGeneratorOffline::setSeed(const double& sed)
{
    mySeed=sed;
}

void CustomerGeneratorOffline::generate(const unsigned& num)
{
    if(myWorld==NULL)
        return;
    boost::random::mt19937 rng(mySeed);
    boost::random::uniform_int_distribution<> sample(0,mySamples.size()-1);
    boost::random::uniform_int_distribution<> noise(-myNoise,myNoise);
    boost::random::uniform_01<boost::random::mt19937> nodepicker(rng);
    QVector<Neighborhood*> neighborhoods = myWorld->network()->neigborhoods();
    int vsample = 0;
    lemon::SmartDigraph::Node source = lemon::INVALID;
    lemon::SmartDigraph::Node target = lemon::INVALID;
    for(int i=0;i<num;++i)
    {
        vsample = sample(rng);
        CustomerGeneratorSample* sel = mySamples[vsample];
        while(sel->source() >= neighborhoods.size() || sel->target()>= neighborhoods.size() || neighborhoods[sel->source()]->nodes().size()==0 || neighborhoods[sel->target()]->nodes().size()==0)
        {
           qDebug()<<"CustomerGeneratorOffline: Neighborhood does not exist or is empty";
           vsample = sample(rng);
           sel = mySamples[vsample];
        }
        source=lemon::INVALID;
        target=lemon::INVALID;
        while(source==target)
        {
            source = neighborhoods[sel->source()]->get( nodepicker() );
            target = neighborhoods[sel->target()]->get( nodepicker() );
        }
        Customer* cust = AgentFactory::instance()->newCustomer("Customer "+QString::number(i),source,target,myWorld);//new Customer(i,"Customer "+QString::number(i),source,target,myWorld);
        uint64_t time=0;
        int vnoise = noise(rng)*60;
        if(vnoise<0 && sel->start()< -1*vnoise)
            time=0;
        else
            time=vnoise+sel->start();
        new EventShowUp(cust,time,myWorld->kernel());
    }
}

QVector<lemon::SmartDigraph::Node> CustomerGeneratorOffline::disperseVehicles(const unsigned& num)
{

    if(myWorld==NULL)
        return QVector<lemon::SmartDigraph::Node>();
    boost::random::mt19937 rng(mySeed);
    boost::random::uniform_int_distribution<> sample(0,mySamples.size()-1);
    boost::random::uniform_01<boost::random::mt19937> nodepicker(rng);
    QVector<Neighborhood*> neighborhoods = myWorld->network()->neigborhoods();
    int vsample = 0;
    lemon::SmartDigraph::Node source = lemon::INVALID;
    QVector<lemon::SmartDigraph::Node> nodes;
    nodes.resize(num);
    for(int i=0;i<num;++i)
    {
        vsample = sample(rng);
        CustomerGeneratorSample* sel = mySamples[vsample];
        while(sel->source() >= neighborhoods.size()  || neighborhoods[sel->source()]->nodes().size()==0)
        {
           qDebug()<<"CustomerGeneratorOffline: Neighborhood does not exist or is empty";
           vsample = sample(rng);
           sel = mySamples[vsample];
        }
        source = neighborhoods[sel->source()]->get( nodepicker() );
        nodes[i]=source;
    }
    return nodes;
}

QVector<CustomerGeneratorSample*> CustomerGeneratorOffline::samples() const
{
   return mySamples;
}
