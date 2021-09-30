#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include "symbol.h"
#include "token.h"
#include "memmap.h"
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

vector<Token> getTokens(char *fn)
{
    vector<Token> tokens;
    ifstream fin(fn);
    string line;
    char *token;
    char delim[] = " \t";
    linenum = 1;
    // tokenizer genereate a token list. The list is for parser only and will not pass to pass2.
    while (getline(fin, line))
    {
        char *temp = strdup(line.c_str());
        char *remaining;
        token = strtok(temp, delim);
        lineoffset = 1;
        while (token != NULL)
        {
            Token t(linenum, lineoffset, token);
            tokens.push_back(t);
            remaining = strtok(NULL, "");
            token = strtok(remaining, delim);
            if (token != NULL)
            {
                char *pch;
                pch = strstr(remaining, token);
                lineoffset = lineoffset + t.getLength() + strlen(remaining) - strlen(pch) + 1;
            }
        }
        free(temp);
        linenum++;
    }
    fin.close();
    return tokens;
}

void pass1(char *ifile, char *ofile, SymbolTable &st)
{
    int defcount, usecount, instcount, memCount = 0, modulecount=0;
    int moduleSize[10];
    ofstream fout(ofile, ios::app);
    vector<Token> tokens = getTokens(ifile);

    // parser
    for (vector<Token>::iterator it = tokens.begin(); it != tokens.end();)
    {
        modulecount+=1;
        // def list
        defcount = readInt((*it++).getValue());
        for (int i = 0; i < defcount; i++)
        {
            Symbol s = readSym((*it++).getValue());
            int v = readInt((*it++).getValue());
            s.setOffset(v + memCount);
            s.setValue(v);
            s.setModule(modulecount);
            if (st.isDefined(s))
            {
                st.setMultiDefined(s);
            }
            else
            {
                st.push_back(s);
            }
        }
        // use list
        usecount = readInt((*it++).getValue());
        for (int i = 0; i < usecount; i++)
        {
            Symbol s = readSym((*it++).getValue());
        }

        // program text
        instcount = readInt((*it++).getValue());
        for (int i = 0; i < instcount; i++)
        {
            char mode = readIERA((*it++).getValue());
            int operand = readInt((*it++).getValue());
        }
        memCount += instcount;
        moduleSize[modulecount] = instcount;
    }
    // check too big
    for (vector<Symbol>::iterator it = st.symbolTable.begin(); it != st.symbolTable.end(); it++)
    {
        int mno = (*it).getModule();
        char line[100];
        if ((*it).getValue() > moduleSize[mno] - 1)
        {
            sprintf(line, "Warning: Module %d: %s too big %d (max=%d) assume zero relative", mno, (*it).getName().c_str(), (*it).getValue(), moduleSize[mno] - 1);
            cout << line << endl;
            fout << line << endl;
            int ms = 0;
            for (int i = 0; i < mno; i++)
            {
                ms += moduleSize[i];
            }
            st.setOffset((*it).getName().c_str(), ms);
        }
    }
    fout.close();
    st.print();
    st.write(ofile);
}

void pass2(char *ifile, char *ofile, SymbolTable &st)
{
    ofstream fout(ofile, ios::app);
    cout << "Memory Map" << endl;
    fout << "Memory Map" << endl;
    int moduleCount = 0;
    int defcount, usecount, instcount, memCount = 0;
    vector<Token> tokens = getTokens(ifile);
    MemMap mmap;
    vector<string> warningMsg;
    vector<Symbol> defvec, usevec, instvec;
    for (vector<Token>::iterator it = tokens.begin(); it != tokens.end();)
    {
        moduleCount++;
        // def list
        defcount = readInt((*it++).getValue());
        string defList[defcount];
        for (int i = 0; i < defcount; i++)
        {
            Symbol s = readSym((*it++).getValue());
            int v = readInt((*it++).getValue());
            s.setOffset(v + memCount);
            st.setModule(s.getName(), moduleCount);
            defList[i] = s.getName();
        }
        // use list
        usecount = readInt((*it++).getValue());
        string useList[usecount];
        for (int i = 0; i < usecount; i++)
        {
            Symbol s = readSym((*it++).getValue());
            useList[i] = s.getName();
            st.setUsed(s.getName());
        }

        // program text
        int offset;
        char line[10];
        instcount = readInt((*it++).getValue());
        int instList[instcount];
        for (int i = 0; i < instcount; i++)
        {
            instList[i] = -1;
            char mode = readIERA((*it++).getValue());
            int addr = readInt((*it++).getValue());
            char ml[100];
            MemLine memLine(memCount + i, addr);
            switch (mode)
            {
            case 'I':
            {
                // mmap.push_back(memCount + i, addr);
                // sprintf(ml, "%03d: %04d", memCount+i, addr);
                break;
            }
            case 'E':
            {
                char msg[50];
                instList[i] = addr;
                if (addr % 1000 > usecount - 1)
                {
                    memLine.hasError = true;
                    memLine.errorMsg = "Error: External address exceeds length of uselist; treated as immediate";
                    memLine.addr = addr;
                }
                else if (!st.isDefined(Symbol(useList[addr % 1000].c_str())))
                {
                    Symbol s = Symbol(useList[addr % 1000].c_str());
                    string n = s.getName();
                    memLine.hasError = true;
                    sprintf(msg, "Error: %s is not defined; zero used", n.c_str());
                    memLine.errorMsg = msg;
                    memLine.addr = addr - addr % 1000;
                }
                else
                {
                    offset = st.getOffset(useList[addr % 1000].c_str());
                    memLine.addr = addr - addr % 1000 + offset;
                }
                break;
            }
            case 'R':
            {
                if (addr % 1000 < instcount)
                {
                    memLine.addr = addr + memCount;
                }
                else
                {
                    memLine.hasError = true;
                    memLine.errorMsg = "Error: Relative address exceeds module size; zero used";
                    memLine.addr = addr - addr % 1000 + memCount;
                }
                break;
            }
            case 'A':
            {
                if (addr % 1000 >= 512)
                {
                    memLine.hasError = true;
                    memLine.errorMsg = "Error: Absolute address exceeds machine size; zero used";
                    memLine.addr = addr - addr % 1000;
                }
                break;
            }
            default:
                break;
            }
            sprintf(ml, "%03d: %04d", memLine.line, memLine.addr);
            cout << ml;
            fout << ml;
            if (memLine.hasError)
            {
                cout << " " << memLine.errorMsg;
                fout << " " << memLine.errorMsg;
            }
            cout << endl;
            fout << endl;
        }
        // cout << endl;
        // fout << endl;
        // check referrd but not used;
        bool used[usecount];
        char wmsg[100];
        for (int i = 0; i < usecount; i++)
        {
            used[i] = false;
        }
        for (int i = 0; i < instcount; i++)
        {
            if (instList[i] != -1 && instList[i] % 1000 < usecount)
            {
                used[instList[i] % 1000] = true;
            }
        }
        for (int i = 0; i < usecount; i++)
        {
            if (!used[i])
            {
                sprintf(wmsg, "Warning: Module %d: %s appeared in the uselist but was not actually used", moduleCount, useList[i].c_str());
                cout << wmsg << endl;
                fout << wmsg << endl;
            }
        }

        memCount += instcount;
    }
    cout << endl;
    fout << endl;
    // st.warningMsg = warningMsg;
    // mmap.print();
    st.printWarning();
    // mmap.write(ofile);
    st.writeWarning(ofile);
}

int main(int argc, char *argv[])
{
    // __parseerror(1);
    for (int i = 1; i < 11; i++)
    {
        SymbolTable symbolTable = SymbolTable();
        char inputFile[50];
        char outputFile[50];
        sprintf(inputFile, "./lab1_assign/input-%d\0", i);
        cout << "inputFile " <<inputFile << endl;
        sprintf(outputFile, "./lab1_assign/myoutput/output-%d\0", i);
        pass1(inputFile, outputFile, symbolTable);
        pass2(inputFile, outputFile, symbolTable);
    }
    return 0;
}
