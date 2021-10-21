#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include "scheduler.h"
using namespace std;

#define ofs ofs_num++ % 40001

int verbose = 0, trace = 0, showEventQ = 0, showProcess = 0;
int quantum = 65535;
int ofs_num = 0;
int maxprio = 4;
int time_cpubusy = 0;
int time_iobusy = 0;
int total_Turnaround_time = 0;
int total_CPU_wait_time = 0;
int finishtime;
int num_processes=0;
int n;
bool call_scheduler;
Scheduler *scheduler;
vector<int> randvals;

void readRandomNumbers(char *fname)
{
  ifstream fin(fname);
  string line;
  int i = 0;
  fin >> n;
  while (i < n)
  {
    fin >> line;
    randvals.push_back(atoi(line.c_str()));
    ++i;
  }

  fin.close();
}

int myrandom(int burst)
{
  return 1 + (randvals[ofs] % burst);
}

void getProcessQueue(char *fname, deque<Process *> &procQ)
{
  ifstream fin(fname);
  string str;
  int pid = 0, at, tc, cb, io;
  while (fin >> str)
  {
    at = atoi(str.c_str());
    fin >> str;
    tc = atoi(str.c_str());
    fin >> str;
    cb = atoi(str.c_str());
    fin >> str;
    io = atoi(str.c_str());
    Process *proc = new Process(pid++, at, tc, cb, io);
    proc->staticPriority = myrandom(maxprio);
    proc->dynamicPriority = proc->staticPriority - 1;
    procQ.push_back(proc);
  }
  fin.close();
}

void simulation(DES *des)
{
  Process *proc;
  Process *running_proc = NULL;
  Event *evt = new Event();
  int currentTime=0, timeInPrevState=0, state_start_time=0;
  bool flag = false;
  while (true)
  {
    evt = des->get_event();
    if (evt == NULL)
    {
      finishtime = currentTime;
      break;
    }
    proc = evt->process;
    currentTime = evt->timeStamp;
    timeInPrevState = currentTime - proc->lastStateTime;
    proc->lastStateTime = currentTime;

    switch (evt->transition)
    {
    case TRANS_TO_READY:
      if (evt->oldstate == STATE_BLOCKED)
      {
        time_iobusy += timeInPrevState;
      }
      if (verbose)
        printf("%d %d %d: %s -> %s\n", currentTime, proc->pid, timeInPrevState, printState(evt->oldstate), printState(STATE_READY));
      scheduler->add_process(proc);
      call_scheduler = true;
      break;
    case TRANS_TO_RUN:
      proc->state = STATE_RUNNING;
      running_proc = proc;
      int cpu_burst;
      if (running_proc->cpu_burst_remaining > 0)
      {
        cpu_burst = running_proc->cpu_burst_remaining;
      }
      else
      {
        cpu_burst = myrandom(running_proc->CPUBurst);
        if (running_proc->totalCPUTime - running_proc->CPUUsedTime < cpu_burst)
        {
          cpu_burst = running_proc->totalCPUTime - running_proc->CPUUsedTime;
        }
      }
      proc->CPUWaitTime += timeInPrevState;
      total_CPU_wait_time += timeInPrevState;
      proc->cpu_burst_remaining = cpu_burst;

      if (cpu_burst <= quantum)
      {
        Event *e = new Event;
        e->oldstate = STATE_RUNNING;
        e->transition = TRANS_TO_BLOCK;
        e->timeStamp = currentTime + cpu_burst;
        e->process = running_proc;
        des->insert_sort(e);
      }
      else
      {
        Event *e = new Event;
        e->oldstate = STATE_RUNNING;
        e->transition = TRANS_TO_PREEMPT;
        e->timeStamp = currentTime + quantum;
        e->process = running_proc;
        des->insert_sort(e);
      }
      if (verbose)
        printf("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n", currentTime, proc->pid, timeInPrevState, printState(STATE_READY), printState(STATE_RUNNING), cpu_burst, proc->totalCPUTime - proc->CPUUsedTime, proc->dynamicPriority);
      break;

    case TRANS_TO_BLOCK:
      running_proc = NULL;
      time_cpubusy += timeInPrevState;
      proc->CPUUsedTime += proc->cpu_burst_remaining;
      proc->cpu_burst_remaining -= timeInPrevState;
      if (proc->CPUUsedTime == proc->totalCPUTime)
      {
        proc->finishTime = currentTime;
        proc->state = STATE_DONE;
        proc->turnaroundTime = currentTime - proc->arrivalTime;
        total_Turnaround_time += proc->turnaroundTime;
        if (verbose)
          printf("%d %d %d: %s\n", currentTime, proc->pid, timeInPrevState, printState(STATE_DONE));
      }
      else
      {
        proc->state = STATE_BLOCKED;
        int io_burst = myrandom(proc->IOBurst);
        Event *e = new Event;
        e->oldstate = STATE_BLOCKED;
        e->transition = TRANS_TO_READY;
        e->process = proc;
        e->timeStamp = currentTime + io_burst;
        proc->IOTime += io_burst;
        des->insert_sort(e);
        if (verbose)
          printf("%d %d %d: %s -> %s  ib=%d rem=%d\n", currentTime, proc->pid, timeInPrevState, printState(STATE_RUNNING), printState(STATE_BLOCKED), io_burst, proc->totalCPUTime - proc->CPUUsedTime);
      }
      if (trace)
      {
        int len = (scheduler->runQueue).size();
        printf("SCHED(%d): ", len);
        for (int i = 0; i < len; ++i)
        {
          printf("%d:%d", (scheduler->runQueue)[i]->pid, state_start_time);
        }
        printf("\n");
      }
      call_scheduler = true;
      break;

    case TRANS_TO_PREEMPT:
      proc->cpu_burst_remaining-=timeInPrevState;
      proc->CPUUsedTime += timeInPrevState;
      time_cpubusy += timeInPrevState;
      proc->state = STATE_PREEMPTION;
      scheduler->add_process(proc);
      running_proc = NULL;
      call_scheduler = true;
      if (verbose)
        printf("%d %d %d: %s -> %s  cb=%d rem=%d prio=%d\n", currentTime, proc->pid, timeInPrevState, printState(STATE_RUNNING), printState(STATE_READY), proc->cpu_burst_remaining, proc->totalCPUTime - proc->CPUUsedTime, proc->dynamicPriority);
      break;
    }
    delete evt;
    if (call_scheduler)
    {
      if (des->get_next_event_time() == currentTime) continue;
      call_scheduler = false;
      if (running_proc == NULL)
      {
        running_proc = scheduler->get_next_process();
        if (running_proc == NULL)
          continue;
        Event *e = new Event;
        e->timeStamp = currentTime;
        e->process = running_proc;
        e->oldstate = STATE_READY;
        e->transition = TRANS_TO_RUN;
        des->insert_sort(e);
      }
    }
  };
}


int main(int argc, char **argv)
{
  char *schedspec = NULL;
  int c;
  while ((c = getopt(argc, argv, "vteps:")) != -1)
    switch (c)
    {
    case 'v':
      verbose = 1;
      break;
    case 't':
      trace = 1;
      break;
    case 'e':
      showEventQ = 1;
      break;
    case 'p':
      showProcess = 1;
      break;
    case 's':
    {
      schedspec = optarg;
      string spec(optarg);
      char type = schedspec[0];
      // sscanf(spec.substr(1, spec.size() - 1).c_str(), "%d:%d", &quantum, &maxprio);
      switch (type)
      {
      case 'F':
        scheduler = new FCFS((char*)"FCFS");
        break;
      case 'L':
        scheduler = new LCFS((char*)"LCFS");
        break;
      case 'S':
        scheduler = new SRTF((char*)"SRTF");
        break;
      case 'R':
        {
          sscanf(spec.substr(1, spec.size() - 1).c_str(), "%d", &quantum);
          char name[10];
          int vv = sprintf(name, "RR %d", quantum);
          scheduler = new RR(name);
          break;
        }
      default:
        break;
      }
      break;
    }

    case '?':
      if (optopt == 's')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      abort();
    }
  char *inputfileName = argv[optind], *randfileName = argv[optind + 1];
  readRandomNumbers(randfileName);
  deque<Process *> procQ;
  getProcessQueue(inputfileName, procQ);
  DES des;
  for (int i = 0; i < procQ.size(); ++i)
  {
    Event *evt = new Event();
    evt->process = procQ[i];
    evt->timeStamp = procQ[i]->arrivalTime;
    evt->oldstate = STATE_CREATED;
    evt->transition = TRANS_TO_READY;
    des.insert_sort(evt);
  }

  simulation(&des);
  
  printf("%s\n", scheduler->type);
  for (int i = 0; i < procQ.size(); ++i)
  {
    Process *p = procQ[i];
    printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", p->pid, p->arrivalTime, p->totalCPUTime, p->CPUBurst, p->IOBurst, p->staticPriority, p->finishTime, p->turnaroundTime, p->IOTime, p->CPUWaitTime);
  }

  num_processes = procQ.size();
  double cpu_util   = 100.0 * (time_cpubusy  / (double) finishtime); 
  double io_util    = 100.0 * (time_iobusy   / (double) finishtime); 
  double avg_TT =  (total_Turnaround_time / (double) num_processes);
  double avg_WT =  (total_CPU_wait_time / (double) num_processes);
  double throughput = 100.0 * (num_processes / (double) finishtime); 
  printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", finishtime, cpu_util, io_util, avg_TT, avg_WT, throughput);
  
  return 0;
}