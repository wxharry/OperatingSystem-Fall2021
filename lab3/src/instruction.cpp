#include "instruction.h"
Instruction::Instruction(char cmd, int page)
{
    this->command=cmd;
    this->page = page;
}
