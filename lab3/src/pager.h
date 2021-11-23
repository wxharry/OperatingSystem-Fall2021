typedef struct { 
  int pid;
  int vpage;

  // // other stuff you probably want to add
  bool mapped = false;
  int index;
  unsigned int age : 32;
  // unsigned int timestamp = 0;
} frame_t;

class Pager 
{ 
public:
  virtual frame_t* select_victim_frame() = 0;   // virtual base class
}; 

class FCFS: public Pager
{
public:
  int hand;

  FCFS();
  frame_t* select_victim_frame();
};

class Random: public Pager
{
public:
  int hand;

  Random();
  frame_t* select_victim_frame();
};

class Clock: public Pager
{
public:
  int hand;

  Clock();
  frame_t* select_victim_frame();
};

class NRU: public Pager
{
public:
  int hand;
  long lastCount=0;
  int period=50;

  NRU();
  frame_t* select_victim_frame();
};

class Aging: public Pager
{
public:
  int hand;
  
  Aging();
  frame_t* select_victim_frame();
};
