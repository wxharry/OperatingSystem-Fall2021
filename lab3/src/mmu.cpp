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

using namespace std;

int MAX_FRAMES=128;
int MAX_VPAGES=64;
int ofs;
vector<int> randvals;
 
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
    p.page_table = new pte_t[MAX_VPAGES];
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

 
frame_t *frame_table = new frame_t[MAX_FRAMES]; 
pte_t *page_table = new pte_t[MAX_VPAGES];  // a per process array of fixed size=64 of pte_t  not pte_t pointers ! 
 
// class Pager { 
//   virtual frame_t* select_victim_frame() = 0;   // virtual base class  
// }; 
 
// frame_t *get_frame() { 
//   frame_t *frame = allocate_frame_from_free_list(); 
//   if (frame == NULL) frame = THE_PAGER->select_victim_frame(); 
//        return frame; 
// } 
vector<pair<char, int> > instructions;
vector<Process> processes;
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
        if (!vma_t)
        {
          pte->file_map = vma_t->file_mapped;
          pte->write_protect = vma_t->write_protected;
        }
        // this in reality generates the page fault exception and now you execute 
        // verify this is actually a valid page in a vma if not raise error and next inst  
        // frame_t *newframe = get_frame(); 

        //-> figure out if/what to do with old frame if it was mapped 
        //   see general outline in MM-slides under Lab3 header and writeup below 
        //   see whether and how to bring in the content of the access page. 
      } 
    }
  //       // check write protection 
  //       // simulate instruction execution by hardware by updating the R/M PTE bits  
  //       update_pte(read/modify) bits based on operations.   
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
    case 'o':
      break;
    case 'a':
    {
    //   switch (type)
    //   {
    //   case 'F':
    //     break;
    //   default:
    //     break;
    //   }
      break;
    }

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
  char *infile = argv[optind], *randfile = argv[optind + 1];
  readRandomNumbers(randfile);
  readInputFile(infile, processes, instructions);
  simulation();
  return 0;
}
