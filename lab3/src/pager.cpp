#include "pager.h"
extern int MAX_FRAMES;
extern frame_t *frame_table;
FCFS::FCFS()
{
    int hand = 0;
}

frame_t* FCFS::select_victim_frame(){
    frame_t* victim = &frame_table[hand];
    hand = (hand+1)%MAX_FRAMES;
    return victim;
}

