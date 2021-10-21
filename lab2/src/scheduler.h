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
    FCFS(){
        type = (char* )"FCFS";
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
    LCFS(){
        type = (char* )"LCFS";
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

class LCFS: public Scheduler
{
public:
    void add_process(Process* proc);
    Process* get_next_process();
    LCFS(){
        type = (char* )"LCFS";
    };
};

