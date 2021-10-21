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
