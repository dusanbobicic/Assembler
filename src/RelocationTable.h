#ifndef RELOCATIONTABLE_H
#define RELOCATIONTABLE_H
#include <string>
#include <iostream>
#include <vector>
#include "Symbol.h"
using namespace std;

enum RelType{
    R_386_32,R_386_PC32
};
class RelocationTable{
    public:
    RelocationTable(string section);
    vector<Relocation>& getRelocations(){return relocations;}
    //Relocation& getRelocation(int i);//??
    string getSection(){return section;}
    void setSection(string section){this->section=section;}
    friend ostream& operator<<(ostream& os,const RelocationTable& rt);


    string section;
    vector<Relocation> relocations;

};
class Relocation{
    public:
    Relocation(short offset,RelType relT,Symbol& s);
    friend ostream& operator<<(ostream&os,const Relocation r);
    string getHexOffset()const;
    string getRelTypeText()const;
    Symbol& s;
    short offset;
    RelType relT;
};


#endif