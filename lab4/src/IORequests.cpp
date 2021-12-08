#include "IORequests.h"
#include <cstdio>
using namespace std;

IORequests::IORequests(int id, int arriveTime, int track)
{
    this->id = id;
    this->arriveTime = arriveTime;
    this->track = track;
}

// FIFO
void FIFO::addQueue(IORequests* r)
{
    this->IOQueue.push_back(r);
}

IORequests* FIFO::getNextRequest(int currentTrack)
{
    if (IOQueue.empty())
    {
        return NULL;
    }
    IORequests* r = IOQueue.front();
    IOQueue.pop_front();
    return r;
}

// SSTF
void SSTF::addQueue(IORequests* r)
{
    this->IOQueue.push_back(r);
}

IORequests* SSTF::getNextRequest(int currentTrack)
{
    if (IOQueue.empty())
    {
        return NULL;
    }
    int minIdx=0;
    for (int i = 0; i < IOQueue.size(); i++)
    {
        if(abs(IOQueue[i]->track - currentTrack) < abs(IOQueue[minIdx]->track - currentTrack))
        {
            minIdx = i;
        }
    }
    IORequests* r = IOQueue[minIdx];
    IOQueue.erase(IOQueue.begin()+minIdx);
    return r;
}
