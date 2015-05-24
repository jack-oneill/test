#ifndef AGENT_H
#define AGENT_H
#include <QPointF>
#include <QString>
#include <QPair>
#include <lemon/smart_graph.h>
enum t_Agent {CUSTOMER,VEHICLE};
class World;
class Agent
{
protected:
    lemon::SmartDigraph::Node myPosition;
    QString myName;
    uint64_t myId;
    t_Agent myType;
    World* myWorld;
public:
    Agent(uint64_t,QString,t_Agent,const lemon::SmartDigraph::Node&,World*);
    virtual ~Agent();
    virtual QPair<lemon::SmartDigraph::Node,double> position(const uint64_t& =0);
    t_Agent type();
    QString name();
    uint64_t id();
    World* world();
};

#endif // AGENT_H
