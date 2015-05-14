#include "simulationkernel.h"
#include <QThread>
SimulationKernel::SimulationKernel(QObject *parent) :
    QObject(parent)
{
    myCurrentTime=0;
    myTimeWarp=0;
    myWorld=NULL;
    myEventQueue=new EventQueue();
    mySimulationStopped=true;
    mySimulationPaused=false;
}
SimulationKernel::~SimulationKernel()
{
    delete myEventQueue;
}

void SimulationKernel::setTimeWarp(double warp)
{
    if(warp<0)
        return;
    myTimeWarp=warp;

}

void SimulationKernel::setWorld(World* world)
{
   myWorld=world;
}

World* SimulationKernel::world()
{
   return myWorld;
}

uint64_t SimulationKernel::time() const
{
    return myCurrentTime;
}

void SimulationKernel::visualizeMe(Agent* agent)
{
    Q_UNUSED(agent)
    emit visualize();
}


void SimulationKernel::sleep(uint64_t time)
{
    if(myTimeWarp>0 && time>=myCurrentTime)
    {
        QThread::sleep((unsigned long)myTimeWarp*(time-myCurrentTime));
        emit visualize();
    }
}
void SimulationKernel::processDeltaCycles()
{
    while(myEventQueue->front()!=NULL && myCurrentTime==myEventQueue->front()->time() && !mySimulationStopped)
    {
        myEventQueue->pop()->execute();
    }
}

void SimulationKernel::initializeSimulation()
{
    myWorld->initialize();
}

void SimulationKernel::endSimulation()
{
    //myWorld->reset();
}

void SimulationKernel::eventLoop()
{
    uint64_t delta=0;
    while(!mySimulationStopped && !mySimulationPaused && myEventQueue->size()>0 && myCurrentTime<myTimeLimit)
    {
       delta = myEventQueue->front()->time()-myCurrentTime;
       myCurrentTime=myEventQueue->front()->time();
       processDeltaCycles();
       if(!mySimulationStopped && !mySimulationPaused)
               sleep(delta);
    }
    if(mySimulationStopped)
    {

    }

}

void SimulationKernel::pushEvent(Event* event)
{
   myEventQueue->push(event);
}

void SimulationKernel::removeEvents(Vehicle* vehicle)
{
    myEventQueue->removeEvents(vehicle);
}

void SimulationKernel::setTimeLimit(uint64_t hours)
{
    myTimeLimit=hours*3600;
}

void SimulationKernel::start()
{
    mySimulationStopped=false;
    if(mySimulationPaused)
        pause();
    else
    {
        myCurrentTime=0;
        new EventStartSimulation(this);
        eventLoop();
    }
    emit simulationEnded();
}

void SimulationKernel::pause()
{
    mySimulationPaused=!mySimulationPaused;
    if(!mySimulationPaused)
        eventLoop();
}

void SimulationKernel::stop()
{
    mySimulationStopped=true;
    myEventQueue->clear();
}
