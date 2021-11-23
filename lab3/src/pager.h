typedef struct { 
  int pid;
  int vpage;

  // // other stuff you probably want to add
  bool mapped = false;
  int index;
  // unsigned int age : 32; // idk why this is 32 bit might not need to be
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

class CLOCK: public Pager
{
public:
  int hand;

  CLOCK();
  frame_t* select_victim_frame();
};

