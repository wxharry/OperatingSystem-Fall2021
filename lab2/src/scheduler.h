#include <cstdlib>
#include "event.h"
using namespace std;

class Scheduler
{
public:
    deque<Process*> runQueue;
    char* type;
    virtual void add_process(Process* proc)=0;
    virtual Process* get_next_process()=0;
    // virtual bool test_preempt(Process *p, int curtime)=0;
};

// FCFS
class FCFS: public Scheduler
{
public:
    void add_process(Process* proc);
    Process* get_next_process();
    FCFS(char* type){
        this->type = type;
    };
};

void FCFS::add_process(Process* proc){
    runQueue.push_back(proc);
}


Process* FCFS::get_next_process(){
    if (runQueue.empty())
    {
        return NULL;
    }
    Process *proc = runQueue.front();
    runQueue.pop_front();
    return proc;
}


// LCFS
class LCFS: public Scheduler
{
public:
    void add_process(Process* proc);
    Process* get_next_process();
    LCFS(char* type){
        this->type = type;
    };
};

void LCFS::add_process(Process* proc){
    runQueue.push_back(proc);
}

Process* LCFS::get_next_process(){
    if (runQueue.empty())
    {
        return NULL;
    }
    Process *proc = runQueue.back();
    runQueue.pop_back();
    return proc;
}

// SRTF
class SRTF: public Scheduler
{
public:
    void add_process(Process* proc);
    Process* get_next_process();
    SRTF(char* type){
        this->type = type;
    };
};

void SRTF::add_process(Process* proc){
    int i = 0;
    for (i; i < runQueue.size() && proc->cpu_burst_remaining >= runQueue[i]->cpu_burst_remaining; ++i);
    runQueue.insert(runQueue.begin() + i, proc);
}

Process* SRTF::get_next_process(){
    if (runQueue.empty())
    {
        return NULL;
    }
    Process *proc = runQueue.back();
    runQueue.pop_back();
    return proc;
}

// RR
class RR: public Scheduler
{
public:
    void add_process(Process* proc);
    Process* get_next_process();
    RR(char* type){
        this->type = type;
    };
};

void RR::add_process(Process* proc){
    runQueue.push_back(proc);
}

Process* RR::get_next_process(){
    if (runQueue.empty())
    {
        return NULL;
    }
    Process *proc = runQueue.front();
    runQueue.pop_front();
    return proc;
}

// PRIO 
class PRIO: public Scheduler
{
public:
    deque< deque <Process*> > *activeQueue;
    deque< deque <Process*> > *expiredQueue;
    int maxPrior;
    void add_process(Process* proc);
    Process* get_next_process();
    PRIO(char* type, int maxPrior);
    bool runQIsEmpty();
    void swap();
};

PRIO::PRIO(char* type, int maxPrior){
    this->type = type;
    this->maxPrior = maxPrior;
    activeQueue = new deque< deque <Process*> > (maxPrior);
    expiredQueue = new deque< deque <Process*> > (maxPrior);
};

void PRIO::add_process(Process* proc){
    if (proc->state == STATE_PREEMPTION) -- proc->dynamicPriority;
    proc->state = STATE_READY;    
    if (proc->dynamicPriority < 0)
    {
        int prio = proc->staticPriority - 1;
        proc->dynamicPriority = prio;
        (*expiredQueue)[prio].push_back(proc);
    }
    else
    {
        (*activeQueue)[proc->dynamicPriority].push_back(proc);
    }    
}

bool PRIO::runQIsEmpty(){
    for (int i = 0; i < maxPrior; ++i)
    {
        if (!(*activeQueue)[i].empty())
        {
            return false;
        }
    }
    return true;
}
void PRIO::swap(){
    deque< deque <Process*> > *temp = activeQueue;
    activeQueue = expiredQueue;
    expiredQueue = temp;
}

Process* PRIO::get_next_process(){
    Process* r=NULL;
    if (runQIsEmpty())
    {
        this->swap();
    }
    for (int i = 0; i < maxPrior; ++i)
    {
        if (!(*activeQueue)[i].empty())
        {
            r=(*activeQueue)[i].front();
            (*activeQueue)[i].pop_front();
        }
    }
    return r;
}


