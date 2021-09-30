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
    int defcount, usecount, instcount, memCount = 0;
    vector<Token> tokens = getTokens(ifile);

    // parser
    for (vector<Token>::iterator it = tokens.begin(); it != tokens.end();)
    {
        // def list
        defcount = readInt((*it++).getValue());
        for (int i = 0; i < defcount; i++)
        {
            Symbol s = readSym((*it++).getValue());
            int v = readInt((*it++).getValue());
            s.setOffset(v + memCount);
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
    }
    st.print();
    st.write(ofile);
}

void pass2(char *ifile, char *ofile, SymbolTable &st)
{
    int moduleCount = 0;
    int defcount, usecount, instcount, memCount = 0;
    vector<Token> tokens = getTokens(ifile);
    MemMap mmap;
    vector<pair<int, string> > warningMsg;
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
        for (int i = 0; i < instcount; i++)
        {
            char mode = readIERA((*it++).getValue());
            int addr = readInt((*it++).getValue());
            switch (mode)
            {
            case 'I':
                mmap.push_back(memCount + i, addr);
                break;
            case 'E':
            {
                MemLine ml(memCount + i, addr);
                Symbol s = Symbol(useList[addr % 1000].c_str());
                char msg[50];
                if (addr%1000 > usecount)
                {
                    string n = s.getName();
                    ml.hasError = true;
                    ml.errorMsg = "Error: External address exceeds length of uselist; treated as immediate";
                    ml.addr = addr;
                    ml.name = n;
                }else if (!st.isDefined(s))
                {
                    string n = s.getName();
                    ml.hasError = true;
                    sprintf(msg, "Error: %s is not defined; zero used", n.c_str());
                    ml.errorMsg = msg;
                    ml.addr = addr - addr % 1000;
                    ml.name = n;
                }
                else
                {
                    offset = st.getOffset(useList[addr % 1000].c_str());
                    ml.addr = addr - addr % 1000 + offset;
                }
                mmap.push_back(ml);

                break;
            }
            case 'R':
            {
                MemLine ml(memCount + i, addr);
                if (addr%1000 < instcount)
                {
                    ml.addr = addr + memCount;
                }else
                {
                    ml.hasError = true;
                    ml.errorMsg = "Error: Relative address exceeds module size; zero used";
                    ml.addr = addr - addr%1000 + memCount;
                }
                mmap.push_back(ml);
                break;
            }
            case 'A':
            {
                MemLine ml(memCount + i, addr);
                if (addr % 1000 >= 512)
                {
                    ml.hasError = true;
                    ml.errorMsg = "Error: Absolute address exceeds machine size; zero used";
                    ml.addr = addr - addr % 1000;
                }
                mmap.push_back(ml);
                break;
            }
            default:
                break;
            }
        }
        // check defined but not used;
        bool used[defcount];
        for (int i = 0; i < defcount; i++)
        {
            used[i] = false;
            for (int j = 0; j < usecount; j++)
            {
                if (defList[i] == useList[j])
                {
                    used[i] = true;
                    break;
                }
            }
            if (!used[i])
            {
                warningMsg.push_back(pair<int, string>(moduleCount, defList[i]));
            }
        }

        memCount += instcount;
    }
    mmap.print();
    st.printWarning();
    mmap.write(ofile);
    st.writeWarning(ofile);
    // for (vector<pair<int, string> >::iterator i = warningMsg.begin(); i != warningMsg.end(); i++)
    // {
    //     printf("Warning: Module %d: %s was defined but never used", (*i).first, (*i).second.c_str());

    // }
}

int main(int argc, char *argv[])
{
    // __parseerror(1);
    for (int i = 1; i < 8; i++)
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
