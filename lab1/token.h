#include <stdio.h>
#include <string>
using namespace std;

class Token
{
private:
    int lineNum;
    int lineOffset;
    string value;

public:
    Token(int n, int o, string v);
    void push_back(Token t);
    int getLineNum();
    int getLineOffset();
    string getValue();
    int getLength();
};

Token::Token(int n, int o, string v)
{
    lineNum = n;
    lineOffset = o;
    value = v;
}

class Tokens
{
private:
    vector<Token> tokens;
    vector<Token>::iterator *pin;

public:
    Tokens();
};

Tokens::Tokens()
{
    vector<Token> v;
    tokens = v;
    *pin = v.begin();
}

int Token::getLineNum()
{
    return lineNum;
}
int Token::getLineOffset()
{
    return lineOffset;
}
string Token::getValue()
{
    return value.c_str();
}

int Token::getLength()
{
    return value.length();
}
