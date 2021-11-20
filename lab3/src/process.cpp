#include "process.h"
#include <deque>
#include <iostream>
#include <cstdio>
using namespace std;

VMA::VMA(int svp, int evp, int wp, int fm)
{
  this->start_vpage = svp;
  this->end_vpage = evp;
  this->write_protected = wp;
  this->file_mapped = fm;
}

Process::Process(){

}

void Process::display(){
    for (VMA &v: vmalist)
    {
        printf("%d %d %d %d\n", v.start_vpage, v.end_vpage, v.write_protected, v.file_mapped);
    }
}
