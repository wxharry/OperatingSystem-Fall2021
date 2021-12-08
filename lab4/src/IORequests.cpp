#include "IORequests.h"
#include <cstdio>
#include <iostream>
using namespace std;

int MAX_TRACK = 10000;

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

IORequests* FIFO::getNextRequest(int currentTrack, int direction)
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

IORequests* SSTF::getNextRequest(int currentTrack, int direction)
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

// LOOK
void LOOK::addQueue(IORequests* r)
{
    this->IOQueue.push_back(r);
}

IORequests* LOOK::getNextRequest(int currentTrack, int direction)
{
    if (IOQueue.empty())
    {
        return NULL;
    }
    if (direction == 0)
    {
        IORequests* r =  IOQueue.front();
        IOQueue.pop_front();
        return r;
    }
    int distance = MAX_TRACK;
    int minIdx=-1;
    for (int i = 0; i < IOQueue.size(); i++)
    {
        if(IOQueue[i]->track - currentTrack == 0)
        {
            IORequests* r = IOQueue[i];
            IOQueue.erase(IOQueue.begin()+i);
            return r;
        }
        else if(direction > 0 && IOQueue[i]->track - currentTrack > 0)
        {
            int dist = IOQueue[i]->track - currentTrack;
            if ( dist < distance)
            {
                distance = dist;
                minIdx = i;
            }
        }
        else if (direction < 0 && currentTrack - IOQueue[i]->track > 0)
        {
            int dist = currentTrack - IOQueue[i]->track;
            if ( dist < distance)
            {
                distance = dist;
                minIdx = i;
            }
        }
        
    }
    if (minIdx == -1)
    {
        return getNextRequest(currentTrack, -direction);
    }
    IORequests* r = IOQueue[minIdx];
    IOQueue.erase(IOQueue.begin()+minIdx);
    return r;
}
