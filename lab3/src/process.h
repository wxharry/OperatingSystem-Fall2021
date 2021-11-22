#include <deque>
using namespace std;

typedef struct {
  unsigned int present:1;
  unsigned int referenced:1;
  unsigned int modified:1;
  unsigned int write_protect:1;
  unsigned int file_map:1;
  unsigned int pageout:1;
  unsigned int frameIndex:16;
  unsigned int empty:10;
} pte_t;          // can only be total of 32-bit size and will check on this 


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
    int pid=0;
    unsigned long unmaps=0;
    unsigned long maps=0;
    unsigned long ins=0;
    unsigned long outs=0;
    unsigned long fins=0;
    unsigned long fouts=0;
    unsigned long zeros=0;
    unsigned long segv=0;
    unsigned long segprot=0;
    deque<VMA> vmalist;
    pte_t *page_table;
    Process();
    Process(int id);
    void display();
    VMA* getAccessibleVMA(int vpage);

};