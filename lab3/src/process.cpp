#include "process.h"
#include <deque>
#include <iostream>
#include <cstdio>
using namespace std;

extern int MAX_VPAGES;
VMA::VMA(int svp, int evp, int wp, int fm)
{
  this->start_vpage = svp;
  this->end_vpage = evp;
  this->write_protected = wp;
  this->file_mapped = fm;
}

Process::Process(int id){
  this->pid = id;
  this->page_table = new pte_t[MAX_VPAGES];
  for (int i = 0; i < MAX_VPAGES; i++)
  {
    page_table[i].frameIndex = 0;
    page_table[i].file_map = 0;
    page_table[i].modified=0;
    page_table[i].pageout=0;
    page_table[i].present=0;
    page_table[i].referenced=0;
    page_table[i].write_protect=0;
    page_table[i].empty=0;
  }
}

Process::Process(){
  this->page_table = new pte_t[MAX_VPAGES];
  for (int i = 0; i < MAX_VPAGES; i++)
  {
    page_table[i].frameIndex=0;
    page_table[i].file_map=0;
    page_table[i].modified=0;
    page_table[i].pageout=0;
    page_table[i].present=0;
    page_table[i].referenced=0;
    page_table[i].write_protect=0;
    page_table[i].empty=0;
  }
}

void Process::display(){
    for (VMA &v: vmalist)
    {
        printf("%d %d %d %d\n", v.start_vpage, v.end_vpage, v.write_protected, v.file_mapped);
    }
}
VMA* Process::getAccessibleVMA(int vpage){
  for (auto &vma : this->vmalist)
  {
    if (vma.start_vpage <= vpage && vma.end_vpage >= vpage)
    {
      return &vma;
    }
  }
  return NULL;
}
