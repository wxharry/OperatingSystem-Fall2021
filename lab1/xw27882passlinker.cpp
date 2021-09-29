#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
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

void pass1(char *ifile, char *ofile, SymbolTable &st)
{
    int defcount, usecount, instcount, memCount = 0;
    // pass1
    ifstream fin(ifile);
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
            st.push_back(s);
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
    st.print();
    st.write(ofile);
    cout << "debug" << endl;
    fin.close();
}

void pass2(char *ifile, char *ofile, SymbolTable &st)
{
    int defcount, usecount, instcount, memCount = 0;
    ifstream fin(ifile);
    ofstream fout(ofile, ios::app);
    string str;
    cout << "Memeory Map" << endl;
    fout << "Memeory Map\n";
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
        }
        // use list
        fin >> str;
        usecount = readInt(str);
        string uselist[usecount];
        for (int i = 0; i < usecount; i++)
        {
            fin >> str;
            Symbol s = readSym(str);
            uselist[i] = s.getName();
        }

        // program text
        fin >> str;
        int offset;
        char line[10];

        instcount = readInt(str);

        for (int i = 0; i < instcount; i++)
        {
            fin >> str;
            char mode = readIERA(str);
            fin >> str;
            int operand = readInt(str);
            switch (mode)
            {
            case 'I':
                sprintf(line, "%03d: %04d\n", memCount + i, operand);
                cout << line;
                fout << line;
                break;
            case 'E':
                offset = st.getOffset(uselist[operand % 1000].c_str());
                sprintf(line, "%03d: %04d\n", memCount + i, operand - operand % 1000 + offset);
                cout << line;
                fout << line;
                break;
            case 'R':
                sprintf(line, "%03d: %04d\n", memCount + i, operand + memCount);
                cout << line;
                fout << line;
                break;
            case 'A':
                sprintf(line, "%03d: %04d\n", memCount + i, operand);
                cout << line;
                fout << line;
                break;
            default:
                break;
            }
        }
        memCount += instcount;
    }
}

int main(int argc, char *argv[])
{
    // __parseerror(1);
    for (int i = 1; i < 4; i++)
    {
        SymbolTable symbolTable = SymbolTable();
        char inputFile[30];
        char outputFile[30];
        sprintf(inputFile, "./lab1_assign/input-%d\0", i);
        sprintf(outputFile, "./lab1_assign/myoutput/output-%d\0", i);
        pass1(inputFile, outputFile, symbolTable);
        pass2(inputFile, outputFile, symbolTable);
    }
    return 0;
}
