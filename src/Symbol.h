#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <iostream>
#include <queue>
#include "Enums.h"

using namespace std;

class Symbol{
    public:
    Symbol(string name, string secs,SectionLabel sec,int offset,Scope sc,TokenType st,int size,int number=0);
    string name,secs;
    SectionLabel sec;
    vector<string> expression;
    int offset;
    Scope sc;
    TokenType st;
    int size;
    int number;
	int flag;
    bool defined;
    bool isEqu;
    friend ostream& operator<<(ostream& os,const Symbol s);
    
};

#endif