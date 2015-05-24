#include "regressionrunner.h"
#include <QDebug>
#include "agentfactory.h"
#include "simulationkernel.h"
#include "outputgenerator.h"
#include  <QTime>
#include <QTextStream>
#include "customergeneratoroffline.h"
#include <QVector>
RegressionRunner::RegressionRunner(SimulationKernel* kernel, CustomerGeneratorOffline* generator, QObject *parent) : QObject(parent)
{
    myKernel=kernel;
    myGenerator=generator;
}
RegressionRunner::~RegressionRunner()
{

}

bool RegressionRunner::load(const QString& file)
{
    QFile data(file);
    if(!data.open(QIODevice::ReadOnly))
    {
        qDebug()<<"RegressionRunner: Could not open regression file "+file;
        return false;
    }
    QTextStream stream(&data);
    QString line;
    QStringList tokens;
    bool isInt=true;
    int vehicles = 0, customers =0, size = 0;
    uint64_t lNum=0;
    while(!stream.atEnd())
    {
        lNum++;
        line = stream.readLine();
        tokens=line.split(",");
        if(tokens.size()!=3)
        {
            qDebug()<<"RegressionRunner: Wrong number of parameters in line " +QString::number(lNum);
            return false;
        }
        customers = tokens[0].toInt(&isInt);
        vehicles = tokens[1].toInt(&isInt);
        size = tokens[2].toInt(&isInt);
        if(!isInt)
        {
            qDebug()<<"RegressionRunner: Non-integer parameter in line " +QString::number(lNum);
            return false;
        }
        QList<int> params;
        params<<customers<<vehicles<<size;
        mySimulations.append(params);
    }
    return true;

}

unsigned RegressionRunner::numRegressions()
{
   return mySimulations.size();
}

void RegressionRunner::pause()
{

}

void RegressionRunner::start()
{
    for(int i=0;i<mySimulations.size();++i)
    {
        //Generate customers
        myGenerator->generate(mySimulations[i][0]);
        myKernel->setTimeLimit(24);
        //Add vehicles
        QVector<lemon::SmartDigraph::Node> vpos = myGenerator->disperseVehicles(mySimulations[i][1]);
        for(int j=0;j<vpos.size();++j)
        {
            Vehicle* veh = AgentFactory::instance()->newVehicle("Vehicle "+QString::number(j),mySimulations[i][2],vpos[j],myKernel->world());
            myKernel->world()->addVehicle(veh);
        }
        qDebug()<<"RegressionRunner: " +QTime::currentTime().toString()+ " Starting simulation "+QString::number(i);
        OutputGenerator::instance()->open("simulation_run_"+QString::number(i)+"_"+QString::number(mySimulations[i][0])+"_"+QString::number(mySimulations[i][1])+".txt");
        myKernel->start();
        myKernel->stop();
        OutputGenerator::instance()->close();
    }
    stop();
    emit regressionEnded();
}

void RegressionRunner::stop()
{
    mySimulations.clear();
}
