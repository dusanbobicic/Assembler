#include "Symbol.h"

Symbol::Symbol(string name, string secs, SectionLabel sec, int offset, Scope sc, SymbolType st, int size,int number=0)
{
    this->name = name;
    this->secs = secs;
    this->sec = sec;
    this->offset = offset;
    this->sc = sc;
    this->st = st;
    this->size = size;
    this->number=number;
    this->defined=false;
    this->isEqu=false;
}
ostream & operator<<(ostream &os, const Symbol s)
{
    string scope;
    switch (s.sc)
    {
    case LOCAL:
        scope = "local";
        break;
    case GLOBAL: scope = "global";
        break;
    default:
        throw runtime_error("Unknown scope!");
    }
    string sType;
    switch (s.st)
    {
    case SECTION:
        sType = "section";
        break;
    case LABEL:
        sType = "label";
        break;
    default:
        throw runtime_error("Unknown symbol!");
    }
    os <<s.number<<"\t"<< s.name << "\t" << s.secs<<"\t"<< sType << "\t" << s.offset << "\t" << scope << "\t"<<s.size ;
    return os;
}