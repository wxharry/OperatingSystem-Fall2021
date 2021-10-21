#include "process.h"
#include <deque>
using namespace std;
enum Trans_to
{
    TRANS_TO_READY,
    TRANS_TO_RUN,
    TRANS_TO_BLOCK,
    TRANS_TO_PREEMPT
};
class Event
{
public:
    int timeStamp;
    Process *process;
    State oldstate;
    Trans_to transition;

    Event();
};

Event::Event()
{
}

class DES
{
public:
    deque<Event*> eventQueue;

    void insert_sort(Event *evt);
    Event* get_event();
    int get_next_event_time();
};


void DES::insert_sort(Event *evt)
{
    int i = 0;
    for (i; i < eventQueue.size() && evt->timeStamp >= eventQueue[i]->timeStamp; ++i);
    eventQueue.insert(eventQueue.begin() + i, evt);
}

Event* DES::get_event()
{
    if (eventQueue.empty())
        return NULL;
    Event *e = eventQueue.front();
    eventQueue.pop_front();
    return e;
}

int DES::get_next_event_time()
{
    return eventQueue.front()->timeStamp;
}
