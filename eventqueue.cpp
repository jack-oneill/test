#include "eventqueue.h"
#include <QDebug>
#include "gui/logger.h"


EventQueue::EventQueue()
{

}
EventQueue::~EventQueue()
{
    clear();
}

Event* EventQueue::pop()
{
    if(myQueue.size()==0)
        return NULL;
    Event* out = myQueue.begin().value().first();
    myQueue.begin().value().removeAll(out);
    if(myQueue.begin().value().size()==0)
        myQueue.erase(myQueue.begin());
    Agent* agent=out->agent();
    if(myAgentEvents.contains(agent))
    {
        myAgentEvents[agent].removeAll(out);
    }
    return out;
}
Event* EventQueue::front() const
{
    if(myQueue.size()==0)
        return NULL;
    if(myQueue.begin().value().size()==0)
        return NULL;
    return myQueue.begin().value().first();
}
void EventQueue::push(Event* event)
{
    if(event==NULL)
        return;

    //std::set<Event*,CompareEventPointers>::iterator fit = myQueue.find(event);
    QMap<uint64_t,QList<Event*> >::iterator fit = myQueue.find(event->time());
    if(fit!=myQueue.end() && fit.value().contains(event))
    {
        Logger::instance()<<80<<fit.key()<<"Error event exists! (This: "+QString::number((uint64_t)event,16)+" time:"+QString::number(event->time())+", found.";
        return;
    }
    myQueue[event->time()].append(event);
    Agent* agent=event->agent();
    if (!myAgentEvents.contains(agent))
        myAgentEvents[agent]=QList<Event*>();
    myAgentEvents[agent].push_back(event);
}

void EventQueue::removeEvents(Agent* agent)
{
    if(myAgentEvents.contains(agent))
    {
        QList<Event*> events = myAgentEvents[agent];
        myAgentEvents[agent].clear();
        QVector<uint64_t> times;
        times.resize(events.size());
        for(int i=0;i<events.size();++i)
        {
            times[i]=events[i]->time();
            myQueue[events[i]->time()].removeAll(events[i]);
            delete events[i];
        }
        for(int i=0;i<times.size();++i)
        {
            if(myQueue[times[i]].size()==0)
                myQueue.remove(times[i]);
        }
    }
}

void EventQueue::removeEvent(Event* event)
{
    if(event==NULL) return;
    Agent* agent = event->agent();
    if(myAgentEvents.contains(agent))
    {
        myAgentEvents[agent].removeAll(event);
        myQueue[event->time()].removeAll(event);
        if(myQueue[event->time()].size()==0)
            myQueue.remove(event->time());
        delete event;
    }
}

unsigned EventQueue::size() const
{
   return myQueue.size();
}

void EventQueue::clear()
{
    for(QMap<uint64_t,QList<Event*> >::iterator it=myQueue.begin();it!=myQueue.end();++it)
    {

        for(int i=0;i<it.value().size();++i)
           delete it.value()[i];
    }
    myQueue.clear();
    myAgentEvents.clear();
}
