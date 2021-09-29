#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
// #include "token.h"
#include "symbol.h"
using namespace std;

int linenum;
int lineoffset;
void __parseerror(int errcode)
{
    static char *errstr[] = {
        (char *)"NUM_EXPECTED",           // Number expect, anything >= 2^30 is not a number either
        (char *)"SYM_EXPECTED",           // Symbol Expected
        (char *)"ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
        (char *)"SYM_TOO_LONG",           // Symbol Name is too long
        (char *)"TOO_MANY_DEF_IN_MODULE", // > 16
        (char *)"TOO_MANY_USE_IN_MODULE", // > 16
        (char *)"TOO_MANY_INSTR",         // total num_instr exceeds memory size (512)
    };
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}

int readInt(string str)
{
    return atoi(str.c_str());
}

// readSym reads a Symbol type from the front of a vector.
Symbol readSym(string str)
{
    return Symbol(str);
}

char readIERA(string str)
{
    return str.c_str()[0];
}

int main(int argc, char *argv[])
{
    // __parseerror(1);
    char filename[] = "./lab1_assign/input-1";
    SymbolTable symbolTable = SymbolTable();
    int defcount, usecount, instcount, memCount = 0;

    ifstream fin(filename);
    string str;
    while (fin >> str)
    {
        // Eliminate a duplicate str at the end
        if (fin.eof())
            break;

        defcount = readInt(str);
        for (int i = 0; i < defcount; i++)
        {
            fin >> str;
            Symbol s = readSym(str);
            fin >> str;
            int v = readInt(str);
            s.setOffset(v + memCount);
            symbolTable.push_back(s);
        }
        // use list
        fin >> str;
        usecount = readInt(str);
        for (int i = 0; i < usecount; i++)
        {
            fin >> str;
            Symbol s = readSym(str);
        }

        // program text
        fin >> str;
        instcount = readInt(str);
        for (int i = 0; i < instcount; i++)
        {
            fin >> str;
            char mode = readIERA(str);
            fin >> str;
            int operand = readInt(str);
        }
        memCount += instcount;
    }
    cout << "Symbol Table" << endl;
    symbolTable.print();
    return 0;
}
