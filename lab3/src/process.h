#include <deque>
using namespace std;

typedef struct {
  unsigned present;
  unsigned referenced;
  unsigned modified;
  unsigned write_protect;
  unsigned file_map;
  unsigned pageout;
} pte_t;          // can only be total of 32-bit size and will check on this 


typedef struct { 
  int pid;
  int vpage;

  // // other stuff you probably want to add
  // bool mapped = false;
  // int index;
  // unsigned int age : 32; // idk why this is 32 bit might not need to be
  // unsigned int timestamp = 0;
 } frame_t;

class VMA
{
public:
  int start_vpage;
  int end_vpage;
  int write_protected;
  int file_mapped;
  VMA(int svp, int evp, int wp, int fm);
};

class Process
{
public:
    int pid;
    deque<VMA> vmalist;
    pte_t *page_table;
    Process();
    Process(int id);
    void display();
    VMA* getAccessibleVMA(int vpage);
};