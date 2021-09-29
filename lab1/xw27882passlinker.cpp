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
        defcount = readInt((*it++).getValue());
        for (int i = 0; i < defcount; i++)
        {
            Symbol s = readSym((*it++).getValue());
            int v = readInt((*it++).getValue());
            s.setOffset(v + memCount);
            st.push_back(s);
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
    int defcount, usecount, instcount, memCount = 0;
    vector<Token> tokens = getTokens(ifile);
    MemMap mmap;
    for (vector<Token>::iterator it = tokens.begin(); it != tokens.end();)
    {
        defcount = readInt((*it++).getValue());
        for (int i = 0; i < defcount; i++)
        {
            Symbol s = readSym((*it++).getValue());
            int v = readInt((*it++).getValue());
            s.setOffset(v + memCount);
            st.push_back(s);
        }
        // use list
        usecount = readInt((*it++).getValue());
        string uselist[usecount];
        for (int i = 0; i < usecount; i++)
        {
            Symbol s = readSym((*it++).getValue());
            uselist[i] = s.getName();
        }

        // program text
        int offset;
        char line[10];
        instcount = readInt((*it++).getValue());
        for (int i = 0; i < instcount; i++)
        {
            char mode = readIERA((*it++).getValue());
            int operand = readInt((*it++).getValue());
            switch (mode)
            {
            case 'I':
                sprintf(line, "%03d: %04d\n", memCount + i, operand);
                mmap.push_back(memCount + i, operand);
                break;
            case 'E':
                offset = st.getOffset(uselist[operand % 1000].c_str());
                mmap.push_back(memCount + i, operand - operand % 1000 + offset);
                break;
            case 'R':
                mmap.push_back(memCount + i, operand + memCount);
                break;
            case 'A':
                mmap.push_back(memCount + i, operand);
                break;
            default:
                break;
            }
        }
        memCount += instcount;
    }
    mmap.print();
    mmap.write(ofile);
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
