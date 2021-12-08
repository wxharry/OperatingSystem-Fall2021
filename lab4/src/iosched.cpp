#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <vector>
#include <deque>
#include <fstream>
#include "IORequests.h"
using namespace std;

IOScheduler *scheduler;
bool VERBOSE = false;
bool DEBUGGING = false;
deque<IORequests> requests;
int total_time=0;
int tot_movement=0;
float avg_turnaround=0;
float avg_waittime=0;
int max_waittime=0;
double tot_waittime=0;
double tot_turnaround=0;

int currentTrack = 0;
int direction=0;
void readInputFile(char *filename)
{
    ifstream fin(filename);
    string line;
    int i=0;
    while (getline(fin, line))
    {
        if(line[0] == '#') continue;
        int ts, track;
        sscanf(line.c_str(), "%d %d", &ts, &track);
        IORequests req(i++, ts, track);
        requests.push_back(req);
    } 
}

void simulation()
{
    int i=0;
    int currentTime = 0;
    IORequests* currentRequest=NULL;
    char ch='\n';
    int target=-1;
    while (true)
    {
        if(DEBUGGING){
            cout << "current time " << currentTime << endl;
            cout << "current track " << currentTrack << endl;
            cout << "current direction " << direction << endl;
            if(currentRequest)
                cout << "current request " << currentRequest->id << endl;
        }
        if (currentTime == requests[i].arriveTime)
        {
            IORequests* r = &requests[i++];
            scheduler->addQueue(r);
            if(VERBOSE) printf("%5d: %5d add %5d\n", currentTime, r->id, r->track);
        }
        // finish current request
        if (currentRequest && currentRequest->track == currentTrack)
        {
            currentRequest->endTime = currentTime;
            if(VERBOSE) printf("%5d: %5d finish %5d\n", currentTime, currentRequest->id, currentRequest->track);
            currentRequest = NULL;
        }
        if (!currentRequest)
        {
            // issue a new request
            if (!(scheduler->IOQueue.empty()))
            {
                currentRequest = scheduler->getNextRequest();
                currentRequest->startTime = currentTime;
                if(VERBOSE) printf("%5d: %5d issue %5d %5d\n", currentTime, currentRequest->id, currentRequest->track, currentTrack);
                continue;
            }
            else if(i == requests.size())
            {
                total_time = currentTime;
                break;
            }
        }
        if (currentRequest)
        {
            if (currentRequest->track > currentTrack)
            {
                ++currentTrack;
            }
            else
            {
                --currentTrack;
            }
            ++ tot_movement;
        }
        ++currentTime;
        if (DEBUGGING)
        if(ch != 'l' && target < currentTime)
        {
            string line="";
            while (ch = getchar())
            {
                if(ch == '\n') break;
                if (ch == 'e'){
                    DEBUGGING = false;
                    ch = 'e';
                    break;
                }
                else if (ch == 'l')
                {
                    ch = 'l';
                    break;
                }
                else if(isdigit(ch)){
                    line += ch;
                }
                else if (ch == 'q')
                {
                    exit(0);
                }
                
            }
            if(line != "")target=stoi(line);
        }
        else{
            cout << endl;
        }
    }
}

int main(int argc, char **argv)
{
    int c;
    while ((c = getopt(argc, argv, "s:vqfd")) != -1)
        switch (c)
        {
        case 's':
        {
            string type(optarg);
            switch (type[0])
            {
                  case 'i':{scheduler = new FIFO;break;}
                  case 'j':{scheduler = new SSTF;break;}
                  case 's':{scheduler = new LOOK;break;}
                  case 'c':{scheduler = new CLOOK;break;}
                //   case 'f':{THE_PAGER = new NRU;break;}
            }
            break;
        }
        case 'v':
            VERBOSE = true;
        case 'q':
        case 'f':
            break;
        case 'd':
            DEBUGGING = true;break;
        case '?':
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort();
        }
    
    char *infile = argv[optind];
    readInputFile(infile);
    deque<IORequests> requests_source = requests;
    simulation();
    for (int i = 0; i < requests.size(); i++)
    {
        auto req = &requests[i];
        tot_turnaround += req->endTime-req->arriveTime;
        int wt = req->startTime-req->arriveTime;
        tot_waittime += wt;
        max_waittime = max(max_waittime, wt);
        printf("%5d: %5d %5d %5d\n",i, req->arriveTime, req->startTime, req->endTime); 
    }
    avg_turnaround = tot_turnaround/requests.size();
    avg_waittime = tot_waittime/requests.size();
    printf("SUM: %d %d %.2lf %.2lf %d\n", 
              total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);
    return 0;
}
