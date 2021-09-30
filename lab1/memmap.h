#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class MemLine
{
public:
    int line; //value is 0 by default.
    int addr;
    bool hasError;
    string errorMsg;

    MemLine(int l, int i);
    int getLine();
    int getaddr();
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

MemLine::MemLine(int l, int i)
{
    line = l;
    addr = i;
}

int MemLine::getLine()
{
    return line;
}

int MemLine::getaddr()
{
    return addr;
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
        char line[100];
        printf("%03d: %04d", (*i).getLine(), (*i).getaddr());
        if ((*i).hasError)
        {
            cout << " " << (*i).errorMsg;
        }
        cout << endl;
    }
}

void MemMap::write(char *fn)
{
    ofstream fout(fn, ios::app);
    char line[100];
    fout << "Memory Map\n";
    for (vector<MemLine>::iterator i = memMap.begin(); i != memMap.end(); i++)
    {
        char line[100];
        sprintf(line, "%03d: %04d", (*i).getLine(), (*i).getaddr());
        fout << line;
        if ((*i).hasError)
        {
            fout << " " << (*i).errorMsg;
        }
        fout << endl;
    }
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
