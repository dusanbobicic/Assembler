#include "Symbol.h"

Symbol::Symbol(string name, string secs, SectionLabel sec, int offset, Scope sc, TokenType st, int size,int number)
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
	this->flag = 0;
}
ostream & operator<<(ostream &os, const Symbol s)
{
	string rwx="";
	if (s.flag & 0x04) {
		rwx += "R";
	}
	if (s.flag & 0x02) {
		rwx += "W";
	}
	if (s.flag & 0x01) {
		rwx += "X";
	}
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
    os <<s.number<<"\t"<< s.name << "\t" << s.secs<<"\t"<< sType << "\t" << s.offset << "\t" << scope << "\t"<<s.size<<"\t"<<rwx;
    return os;
}