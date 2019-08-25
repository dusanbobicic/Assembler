#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <map>
#include <string>
#include "Symbol.h"

using namespace std;
class SymbolTable
{
public:
    map<string, Symbol> symbols;
    SymbolTable();
    bool addNewSymbol(Symbol &s);
    ~SymbolTable(){
        //--implement!
    }
    friend ostream &operator<<(ostream &os, const SymbolTable& st);
    int counter;

private:
};

#endif