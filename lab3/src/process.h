#include <deque>
using namespace std;

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
    deque<VMA> vmalist;
    Process();
    void display();
};