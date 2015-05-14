#include "eventqueue.h"
#include "gui/logger.h"


EventQueue::EventQueue()
{

}
EventQueue::~EventQueue()
{
    for(std::set<Event*,CompareEventPointers>::iterator it = myQueue.begin(); it!=myQueue.end();++it)
    {
        delete *it;
    }
}

Event* EventQueue::pop()
{
    if(myQueue.size()==0)
        return NULL;
    Event* out = *myQueue.begin();
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
    return *myQueue.begin();
}
void EventQueue::push(Event* event)
{
    if(event==NULL)
        return;

    std::multiset<Event*,CompareEventPointers>::iterator fit = myQueue.find(event);
    if(*fit==event)
    {
        Logger::instance()<<80<<(*fit)->time()<<"Error event exists! (This: "+QString::number((uint64_t)event,16)+" time:"+QString::number(event->time())+", found "+QString::number((uint64_t)(*fit),16)+" time"+QString::number((*fit)->time())+")";
        return;
    }
    myQueue.insert(event);
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
        for(int i=0;i<events.size();++i)
        {
            myQueue.erase(events[i]);
        }
    }
}

void EventQueue::removeEvent(Event* event)
{
    if(event==NULL) return;
    Agent* agent = event->agent();
    if(myAgentEvents.contains(agent))
    {
        QList<Event*> events = myAgentEvents[agent];
        myAgentEvents[agent].clear();
        for(int i=0;i<events.size();++i)
        {
            myQueue.erase(events[i]);
        }
    }
}

unsigned EventQueue::size() const
{
   return myQueue.size();
}

void EventQueue::clear()
{
    for(std::set<Event*>::iterator it=myQueue.begin();it!=myQueue.end();++it)
        delete *it;
    myQueue.clear();
    myAgentEvents.clear();
}
