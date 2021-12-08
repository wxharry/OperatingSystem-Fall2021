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
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
};

class SSTF: public IOScheduler
{
public:
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
};

class LOOK: public IOScheduler
{
public:
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
};

class CLOOK: public IOScheduler
{
public:
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
};

