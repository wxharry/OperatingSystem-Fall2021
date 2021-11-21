#include <deque>
using namespace std;

typedef struct {
  unsigned present;
  unsigned referenced;
  unsigned modified;
  unsigned write_protect;
  unsigned file_map;
  unsigned pageout;
  unsigned frameIndex;
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