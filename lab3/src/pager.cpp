#include "pager.h"
#include "process.h"
#include <vector>
#include <stdio.h>
extern int MAX_FRAMES;
extern frame_t *frame_table;
extern vector<Process> processes;
extern int myrandom(int num);
FCFS::FCFS()
{
    int hand = 0;
}

frame_t* FCFS::select_victim_frame(){
    frame_t* victim = &frame_table[hand];
    hand = (hand+1)%MAX_FRAMES;
    return victim;
}

Random::Random(){
    int hand = 0;
}

frame_t* Random::select_victim_frame (){
    hand = myrandom(MAX_FRAMES);
    // printf("%d %d\n", hand, MAX_FRAMES);
    frame_t* victim = &frame_table[hand];
    return victim;
}

Clock::Clock()
{
    int hand=0;
}

frame_t* Clock::select_victim_frame(){
    while (true) {
        frame_t* f=&frame_table[hand];
        Process p=processes[f->pid];
        unsigned int R = p.page_table[f->vpage].referenced;
        if (R==0) {
            hand = (hand+1)%MAX_FRAMES;
            return f;
        }
        else {
            processes[f->pid].page_table[f->vpage].referenced=0;
            hand = (hand+1)%MAX_FRAMES;
        }
    }
}

