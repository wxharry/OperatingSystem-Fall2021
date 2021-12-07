#include <deque>
#include <string>

using namespace std;

class IORequests
{
public:
    int id;
    int arriveTime;
    int startTime;
    int endTime;
    int track;
    IORequests(int id, int arriveTime, int track);
};

class IOScheduler
{
public:
    deque<IORequests* > IOQueue;
    string type;
    virtual void addQueue(IORequests* r)=0;
    virtual IORequests* getNextRequest()=0;
};

class FIFO: public IOScheduler
{
public:
    string type = "FIFO";
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
};
