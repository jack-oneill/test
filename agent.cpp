#include "agent.h"
#include "simulationkernel.h"

Agent::Agent(uint64_t id, QString name, t_Agent type, const lemon::SmartDigraph::Node& pos, World *world)
{
    myId=id;
    myName=name;
    myType=type;
    myPosition=pos;
    myWorld=world;
}
Agent::~Agent()
{

}

QPair<lemon::SmartDigraph::Node,double> Agent::position(const uint64_t& t)
{
    Q_UNUSED(t)
    return QPair<lemon::SmartDigraph::Node,double>(myPosition,0);
}

t_Agent Agent::type()
{
    return myType;
}

uint64_t Agent::id()
{
    return myId;
}
World* Agent::world()
{
    return myWorld;
}
