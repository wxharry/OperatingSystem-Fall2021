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
    virtual void addQueue(IORequests* r)=0;
    virtual IORequests* getNextRequest()=0;
    virtual bool isPending()=0;
};

class FIFO: public IOScheduler
{
public:
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
    virtual bool isPending();
};

class SSTF: public IOScheduler
{
public:
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
    virtual bool isPending();
};

class LOOK: public IOScheduler
{
public:
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
    virtual bool isPending();
};

class CLOOK: public IOScheduler
{
public:
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
    virtual bool isPending();
};

class FLOOK: public IOScheduler
{
public:
    deque<IORequests* > AddQueue;
    void addQueue(IORequests* r);
    IORequests* getNextRequest();
    virtual bool isPending();
};
