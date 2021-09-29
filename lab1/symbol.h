#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class Symbol
{
private:
    string name;
    int value; //value is 0 by default.
    int offset;
    bool used;
    int moduleNum;

public:
    Symbol(string n);
    Symbol(string n, int v);
    Symbol(string n, int v, int m);
    string getName();
    int getValue();
    void setValue(int v);
    int getOffset();
    void setOffset(int v);
    void setUsed();
    bool getUsed();
    void setModule(int);
    int getModule();
};

class SymbolTable
{
private:
    vector<Symbol> symbolTable;

public:
    SymbolTable();
    void print();
    void printWarning();
    void push_back(Symbol s);
    int getOffset(string n);
    void write(char *fn);
    void writeWarning(char *fn);
    void setUsed(string n);
    void setModule(string, int);
};

Symbol::Symbol(string n, int v)
{
    name = n;
    value = v;
    used = false;
}

Symbol::Symbol(string n)
{
    name = n;
    value = 0;
    used = false;
}
Symbol::Symbol(string n, int v, int m)
{
    name = n;
    value = 0;
    used = false;
    moduleNum = m;
}

string Symbol::getName()
{
    return name;
}

int Symbol::getValue()
{
    return value;
}

void Symbol::setValue(int v)
{
    value = v;
}

int Symbol::getOffset()
{
    return offset;
}

void Symbol::setOffset(int v)
{
    offset = v;
}

void Symbol::setUsed()
{
    used = true;
}
bool Symbol::getUsed()
{
    return used;
}
void Symbol::setModule(int n)
{
    moduleNum = n;
}
int Symbol::getModule()
{
    return moduleNum;
}

bool isSymNameValid(string n)
{
    if (!isalpha(n[0]))
    {
        return false;
    }
    for (string::iterator i = n.begin() + 1; i != n.end(); i++)
    {
        if (!isalpha(*i) || !isdigit(*i))
        {
            return false;
        }
    }
    return true;
}

SymbolTable::SymbolTable()
{
    vector<Symbol> t;
    symbolTable = t;
}

void SymbolTable::print()
{
    cout << "Symbol Table" << endl;
    for (vector<Symbol>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        printf("%s=%d\n", (*i).getName().c_str(), (*i).getOffset());
    }
    cout << endl;
}

void SymbolTable::printWarning()
{
    for (vector<Symbol>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        if (!(*i).getUsed())
        {
            printf("Warning: Module %d: %s was defined but never used\n", (*i).getModule(), (*i).getName().c_str());
        }
    }
    cout << endl;
}

void SymbolTable::write(char *fn)
{
    ofstream fout(fn);
    char line[50];
    fout << "Symbol Table\n";
    for (vector<Symbol>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        sprintf(line, "%s=%d\n", (*i).getName().c_str(), (*i).getOffset());
        fout << line;
    }
    fout << endl;
    fout.close();
}

void SymbolTable::writeWarning(char *fn)
{
    ofstream fout(fn, ios::app);
    char line[100];
    for (vector<Symbol>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        if (!(*i).getUsed())
        {
            sprintf(line, "Warning: Module %d: %s was defined but never used\n", (*i).getModule(), (*i).getName().c_str());
            fout << line;
        }
    }
    fout << endl;
    fout.close();
}
void SymbolTable::push_back(Symbol s)
{
    symbolTable.push_back(s);
}

int SymbolTable::getOffset(string n)
{
    for (vector<Symbol>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        if ((*i).getName().c_str() == n)
        {
            return (*i).getOffset();
        }
    }
    return -1;
}

void SymbolTable::setUsed(string n)
{
    for (vector<Symbol>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        if ((*i).getName().c_str() == n)
        {
            return (*i).setUsed();
        }
    }
}

void SymbolTable::setModule(string n, int m)
{
    for (vector<Symbol>::iterator i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        if ((*i).getName().c_str() == n)
        {
            (*i).setModule(m);
        }
    }
}
