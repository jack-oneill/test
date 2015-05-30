#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H
#include <set>
#include <QMap>
#include <QList>
#include "agent.h"
#include "event.h"

struct CompareEventPointers
{
    bool operator()(Event* A,Event* B)
    {
        return A->time()<B->time();
    }
};

class EventQueue
{
private:
    //std::set<Event*,CompareEventPointers> myQueue;
    QMap<Agent*,QList<Event*> > myAgentEvents;
    QMap<uint64_t,QList<Event*> > myQueue;
public:
    EventQueue();
    ~EventQueue();
    Event* pop() ;
    Event* front() const;
    void push(Event*);
    void removeEvents(Agent*);
    void removeEvent(Event*);
    unsigned size() const;
    void clear();
    bool existsEvent(const uint64_t&,const EventType&,Agent* ag) const;
};

#endif // EVENTQUEUE_H
