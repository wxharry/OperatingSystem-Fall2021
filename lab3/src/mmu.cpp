#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "process.h"
#include "pager.h"

using namespace std;

int MAX_FRAMES=128;
int MAX_VPAGES=64;
int ofs;
vector<int> randvals;
vector<pair<char, int> > instructions;
vector<Process> processes;
Pager *THE_PAGER;
pte_t *page_table = new pte_t[MAX_VPAGES];  // a per process array of fixed size=64 of pte_t  not pte_t pointers ! 
frame_t *frame_table;
deque<int> freelist;

void readRandomNumbers(char *fname)
{
  ifstream fin(fname);
  string line;
  int i = 0;
  fin >> ofs;
  while (i < ofs)
  {
    fin >> line;
    randvals.push_back(atoi(line.c_str()));
    ++i;
  }

  fin.close();
}

int myrandom(int num)
{
  return 1 + (randvals[ofs] % num);
}

void readInputFile(char* filename, vector<Process>& processes, vector<pair<char, int> >& instructions){
  ifstream fin(filename);
  string line;
  int start_vpage, end_vpage, write_protected, file_mapped;
  // read processes
  getline(fin, line);
  while(line[0] == '#') getline(fin, line);
  int processNum = stoi(line);
  for (int i = 0; i < processNum; i++)
  {
    getline(fin, line);
    while(line[0] == '#') getline(fin, line);
    int vmaNum = stoi(line);
    Process p(i);
    // read vma for each process
    for (int j = 0; j < vmaNum; j++)
    {
      getline(fin, line);
      sscanf(line.c_str(), "%d %d %d %d", &start_vpage, &end_vpage, &write_protected, &file_mapped);
      VMA vma(start_vpage, end_vpage, write_protected, file_mapped);
      p.vmalist.push_back(vma);
    }
    processes.push_back(p);
  }
  // read instructions
  while(line[0] == '#') getline(fin, line);
  while (getline(fin, line))
  {
    if(line[0] == '#') continue;
    char cmd; int page;
    sscanf(line.c_str(), "%c %d", &cmd, &page);
    instructions.push_back(pair<char, int>({cmd, page}));
  }
  
}

frame_t *allocate_frame_from_free_list(){
  if (freelist.empty()) return NULL;
  else
  {
    int fid = freelist.front();
    freelist.pop_front();
    return &frame_table[fid];
  }
}

frame_t *get_frame() { 
  frame_t *frame = allocate_frame_from_free_list(); 
  if (frame == NULL) frame = THE_PAGER->select_victim_frame();
  return frame; 
} 


void simulation(){
  for (int i=0; i<instructions.size(); i++)
  {
    auto ins = instructions[i];
    printf("%d: ==> %c %d\n", i, ins.first, ins.second);
    Process *current_process;
    int vpage;
    char operation=ins.first;
    if(operation=='c'){
      current_process = &processes[ins.second];
    }
    else if (operation=='e')
    {

    }
    else
    {
      vpage=ins.second;
      pte_t *pte = &current_process->page_table[vpage];
      if (!pte->present) { 
        VMA *vma_t = current_process->getAccessibleVMA(vpage);
        if (vma_t)
        {
          pte->file_map = vma_t->file_mapped;
          pte->write_protect = vma_t->write_protected;
        }
        else
        {
          printf(" SEGV\n");
          continue;
        }
        
        // this in reality generates the page fault exception and now you execute 
        // verify this is actually a valid page in a vma if not raise error and next inst  
        frame_t *newframe = get_frame(); 
        if (newframe->mapped)
        {
          printf(" UNMAP %d:%d\n", newframe->pid, newframe->vpage);
          pte_t _pte = processes[newframe->pid].page_table[newframe->vpage];
          if (_pte.modified) {
            if (_pte.file_map) {
              printf(" FOUT\n");
            }
            else {
              printf(" OUT\n");
            }
          }
        }
        if (pte->pageout)
        {
          printf(" IN\n");
        }
        else if (pte->file_map)
        {
          printf(" FIN\n");
        }
        else
        {
          printf(" ZERO\n");
        }
        pte->frameIndex = newframe->index;
        pte->present = 1;

        newframe->pid = current_process->pid;
        newframe->vpage = vpage;
        newframe->mapped = 1;
        printf(" MAP %d\n", pte->frameIndex);
      } 
      // check write protection 
      // simulate instruction execution by hardware by updating the R/M PTE bits  
      // update_pte(read/modify) bits based on operations.
      if (operation == 'r') {
        pte->referenced = 1;
      }
      else if (operation=='w') {
        pte->referenced=1;
        if (pte->write_protect) {
          printf(" SEGPROT\n");
        }
        else {
          pte->modified = 1;
        }
      }
    }

  }
}

int main(int argc, char **argv)
{
  char *schedspec = NULL;
  int c;
  while ((c = getopt(argc, argv, "f:a:o:")) != -1)
    switch (c)
    {
    case 'f':
        MAX_FRAMES=stoi(optarg);
        break;
    case 'a':
    {
      string type(optarg);
      switch (type[0])
      {
      case 'f':
        THE_PAGER = new FCFS;
        break;
      default:
        break;
      }
      break;
    }
    case 'o':
      break;
    case '?':
      if (optopt == 'o')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      abort();
    }
  frame_table = new frame_t[MAX_FRAMES];
  for (int i = 0; i < MAX_FRAMES; i++)
  {
    frame_table[i].index=i;
    frame_table[i].mapped=0;
    frame_table[i].pid=-1;
    frame_table[i].vpage=-1;
    freelist.push_back(i);
  }
  
  char *infile = argv[optind], *randfile = argv[optind + 1];
  readRandomNumbers(randfile);
  readInputFile(infile, processes, instructions);
  simulation();
  return 0;
}
