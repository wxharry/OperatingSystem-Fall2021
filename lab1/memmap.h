#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class MemLine
{
private:
    string name;
    int line; //value is 0 by default.
    int instruction;

public:
    MemLine(int l, int i);
    int getLine();
    int getInstruction();
};

class MemMap
{
private:
    vector<MemLine> memMap;

public:
    MemMap();
    void push_back(MemLine s);
    void push_back(int l, int i);
    void print();
    void write(char *fn);
};

MemLine::MemLine(int l, int i){
    line = l;
    instruction = i;
}

int MemLine::getLine()
{
    return line;
}

int MemLine::getInstruction()
{
    return instruction;
}

// bool isSymNameValid(string n)
// {
//     if (!isalpha(n[0]))
//     {
//         return false;
//     }
//     for (string::iterator i = n.begin() + 1; i != n.end(); i++)
//     {
//         if (!isalpha(*i) || !isdigit(*i))
//         {
//             return false;
//         }
//     }
//     return true;
// }

MemMap::MemMap()
{
    vector<MemLine> t;
    memMap = t;
}

void MemMap::print()
{
    cout << "Memory Map" << endl;
    for (vector<MemLine>::iterator i = memMap.begin(); i != memMap.end(); i++)
    {
        printf("%03d: %04d\n", (*i).getLine(), (*i).getInstruction());
    }
    cout << endl;
}

void MemMap::write(char *fn)
{
    ofstream fout(fn, ios::app);
    char line[50];
    fout << "Memory Map\n";
    for (vector<MemLine>::iterator i = memMap.begin(); i != memMap.end(); i++)
    {
        sprintf(line, "%03d: %04d\n", (*i).getLine(), (*i).getInstruction());
        fout << line;
    }
    fout << endl;
    fout.close();
}

void MemMap::push_back(MemLine l)
{
    memMap.push_back(l);
}

void MemMap::push_back(int l, int i)
{
    MemLine m(l, i);
    memMap.push_back(m);
}
