#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>
#include <string.h>
#include <vector>
#include <queue>
#include "symbol.h"
#include "token.h"
#include "memmap.h"
using namespace std;

int linenum;
int lineoffset;
char inputFile[50];
char outputFile[50];
void __parseerror(int errcode, Token t)
{
    // ofstream fout(outputFile);
    string errstr[] = {
        "NUM_EXPECTED",           // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED",           // Symbol Expected
        "ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG",           // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR",         // total num_instr exceeds memory size (512)
    };
    char msg[100];
    sprintf(msg, "Parse Error line %d offset %d: %s\n", t.lineNum, t.lineOffset, errstr[errcode].c_str());
    cout << msg << endl;
    // fout msg << endl;
    exit(0);
}

int readInt(Token t)
{
    int result;
    string x = t.getValue().c_str();
    bool flag = true;
    for (int i = 0; i < x.size(); i++)
    {
        flag *= isdigit(x[i]);
    }
    if (flag == true)
    {
        return atoi(t.getValue().c_str());
    }
    else
    {
        __parseerror(0, t);
    }
    return result;
}

// readSym reads a Symbol type from the front of a vector.
Symbol readSym(Token t)
{
    string x = t.getValue();
    if (x.length() > 16)
    {
        __parseerror(3, t);
    }

    for (int i = 0; i < x.length(); i++)
    {
        if (!isalpha(x[0]))
        {
            __parseerror(1, t);
        }
        else if (i > 0 && !isalnum(x[i]))
        {
            __parseerror(1, t);
        }
    }
    return Symbol(x);
}

char readIERA(Token t)
{
    string x = t.getValue();
    // if (x.length() != 2)
    // {
    //     __parseerror(2, t);
    // }
    if (x[0] != 'I' && x[0] != 'A' && x[0] != 'E' && x[0] != 'R')
    {
        __parseerror(2, t);
    }

    return x.c_str()[0];
}

vector<Token> getTokens(char *fn)
{
    vector<Token> tokens;
    ifstream fin(fn);
    string line;
    int len;
    char *token;
    char delim[] = " \t";
    linenum = 1;
    // tokenizer genereate a token list. The list is for parser only and will not pass to pass2.
    while (getline(fin, line))
    {
        len = strlen(&line[0]);
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
    Token end = Token();
    if (linenum == 1 && len == 0)
    {
        end.lineNum = 1;
        end.lineOffset = 1;
        end.value = "";
    }
    else
    {
        end.lineNum = linenum - 1;
        end.lineOffset = len + 1;
        end.value = "";
    }
    tokens.push_back(end);
    return tokens;
}

void pass1(char *ifile, char *ofile, SymbolTable &st)
{
    int defcount, usecount, instcount, memCount = 0, modulecount = 0;
    int moduleSize[10];
    // ofstream fout(ofile, ios::app);
    vector<Token> tokens = getTokens(ifile);
    // parser
    for (int it = 0; it < tokens.size() - 1;)
    {
        modulecount += 1;
        // def list
        defcount = readInt(tokens[it++]);
        if (defcount > 16)
        {
            __parseerror(4, tokens.front());
        }

        for (int i = 0; i < defcount; i++)
        {
            Symbol s;
            s = readSym(tokens[it++]);
            if (it >= tokens.size())
            {
                __parseerror(1, tokens.back());
            }
            int v = readInt(tokens[it++]);
            if (it >= tokens.size())
            {
                __parseerror(0, tokens.back());
            }
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
        usecount = readInt(tokens[it]);
        if (usecount > 16)
        {
            __parseerror(5, tokens[it]);
        }
        it++;

        if (it >= tokens.size())
        {
            __parseerror(0, tokens.back());
        }

        for (int i = 0; i < usecount; i++)
        {
            Symbol s = readSym(tokens[it++]);
        }

        // program text
        instcount = readInt(tokens[it]);
        if (memCount + instcount > 512)
        {
            __parseerror(6, tokens[it]);
        }
        it++;
        for (int i = 0; i < instcount; i++)
        {
            char mode = readIERA(tokens[it++]);
            int operand = readInt(tokens[it++]);
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
            // fout line << endl;
            int ms = 0;
            for (int i = 1; i < mno; i++)
            {
                ms += moduleSize[i];
            }
            st.setOffset((*it).getName().c_str(), ms);
        }
    }
    // fout.close();
    st.print();
    // st.write(ofile);
}

void pass2(char *ifile, char *ofile, SymbolTable &st)
{
    // ofstream fout(ofile, ios::app);
    cout << "Memory Map" << endl;
    // fout "Memory Map" << endl;
    int moduleCount = 0;
    int defcount, usecount, instcount, memCount = 0;
    vector<Token> tokens = getTokens(ifile);
    MemMap mmap;
    vector<string> warningMsg;
    vector<Symbol> defvec, usevec, instvec;
    for (int it = 0; it < tokens.size() - 1;)
    {
        moduleCount++;
        // def list
        defcount = readInt(tokens[it++]);
        string defList[defcount];
        for (int i = 0; i < defcount; i++)
        {
            Symbol s = readSym(tokens[it++]);
            int v = readInt(tokens[it++]);
            s.setOffset(v + memCount);
            defList[i] = s.getName();
        }
        // use list
        usecount = readInt(tokens[it++]);
        string useList[usecount];
        for (int i = 0; i < usecount; i++)
        {
            Symbol s = readSym(tokens[it++]);
            useList[i] = s.getName();
            st.setUsed(s.getName());
        }

        // program text
        int offset;
        char line[10];
        instcount = readInt(tokens[it++]);
        int instList[instcount];
        for (int i = 0; i < instcount; i++)
        {
            instList[i] = -1;
            char mode = readIERA(tokens[it++]);
            int addr = readInt(tokens[it++]);
            char ml[100];
            MemLine memLine(memCount + i, addr);
            memLine.hasError = false;
            if (addr / 1000 > 9)
            {
                memLine.hasError = true;
                memLine.errorMsg = "Error: Illegal opcode; treated as 9999";
                memLine.outAddr = 9999;
            }

            switch (mode)
            {
            case 'I':
            {
                if (memLine.addr > 9999)
                {
                    memLine.hasError = true;
                    memLine.errorMsg = "Error: Illegal immediate value; treated as 9999";
                    memLine.outAddr = 9999;
                }
                break;
            }
            case 'E':
            {
                char msg[50];
                instList[i] = addr;
                if (!memLine.hasError && addr % 1000 > usecount - 1)
                {
                    memLine.hasError = true;
                    memLine.errorMsg = "Error: External address exceeds length of uselist; treated as immediate";
                    memLine.outAddr = addr;
                }
                else if (!st.isDefined(Symbol(useList[addr % 1000].c_str())))
                {
                    Symbol s = Symbol(useList[addr % 1000].c_str());
                    string n = s.getName();
                    memLine.hasError = true;
                    sprintf(msg, "Error: %s is not defined; zero used", n.c_str());
                    memLine.errorMsg = msg;
                    memLine.outAddr = addr - addr % 1000;
                }
                else
                {
                    offset = st.getOffset(useList[addr % 1000].c_str());
                    memLine.outAddr = addr - addr % 1000 + offset;
                }
                break;
            }
            case 'R':
            {
                if (memLine.hasError)
                    break;
                if (addr % 1000 < instcount)
                {
                    memLine.outAddr = addr + memCount;
                }
                else
                {
                    memLine.hasError = true;
                    memLine.errorMsg = "Error: Relative address exceeds module size; zero used";
                    memLine.outAddr = addr - addr % 1000 + memCount;
                }
                break;
            }
            case 'A':
            {
                if (addr % 1000 >= 512)
                {
                    memLine.hasError = true;
                    memLine.errorMsg = "Error: Absolute address exceeds machine size; zero used";
                    memLine.outAddr = addr - addr % 1000;
                }
                break;
            }
            default:
                break;
            }
            sprintf(ml, "%03d: %04d", memLine.line, memLine.outAddr);
            cout << ml;
            // fout ml;
            if (memLine.hasError)
            {
                cout << " " << memLine.errorMsg;
                // fout " " << memLine.errorMsg;
            }
            cout << endl;
            // fout endl;
        }
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
                // fout wmsg << endl;
            }
        }

        memCount += instcount;
    }
    cout << endl;
    // fout endl;
    st.printWarning();
    // st.writeWarning(ofile);
}

int main(int argc, char *argv[])
{
    SymbolTable symbolTable = SymbolTable();
    char *fn = argv[1];
    sprintf(inputFile, "%s", fn);
    sprintf(outputFile, "./lab1_assign/myoutput/output-1");
    pass1(inputFile, NULL, symbolTable);
    pass2(inputFile, NULL, symbolTable);
    return 0;
}
