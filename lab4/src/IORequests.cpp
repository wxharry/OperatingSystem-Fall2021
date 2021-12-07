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

IORequests* FIFO::getNextRequest()
{
    if (IOQueue.empty())
    {
        return NULL;
    }
    IORequests* r = IOQueue.front();
    IOQueue.pop_front();
    return r;
}
