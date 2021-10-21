enum State{STATE_CREATED, STATE_READY, STATE_RUNNING, STATE_BLOCKED, STATE_PREEMPTION, STATE_DONE};

const char* printState(int n){
    std::string states[] = {"CREATED", "READY", "RUNNG", "BLOCK", "PREEMP", "Done"};
    return states[n].c_str();
}

class Process
{
public:
    int pid;
    int arrivalTime;
    int totalCPUTime;
    int CPUBurst;
    int IOBurst;

    State state;
    int staticPriority;
    int dynamicPriority;
    int finishTime;
    int turnaroundTime;
    int IOTime;
    int CPUWaitTime;
    int cpu_burst_remaining;
    int CPUUsedTime;
    int lastStateTime;
    Process(int id, int at, int tc, int cb, int io);
};

Process::Process(int id, int at, int tc, int cb, int io)
{
    pid = id;
    arrivalTime = at;
    totalCPUTime = tc;
    CPUBurst = cb;
    IOBurst = io;
    IOTime=0;
    CPUWaitTime=0;
    cpu_burst_remaining = 0;
    CPUUsedTime = 0;
    lastStateTime = at;
}
