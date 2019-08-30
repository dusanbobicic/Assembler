#include "RelocationTable.h"


RelocationTable::RelocationTable(string section){
    this->section=section;
}

Relocation::Relocation(short offset,RelType relT,Symbol& s):s(s){
    this->offset=offset;
    this->relT=relT;
}
string Relocation::getHexOffset()const {
    //Converts offset into a 4hex String
    char str[10];
    sprintf(str,"%04X",offset);
    string s(str);
    return s;
}
string Relocation::getRelTypeText()const {
    switch(relT){
        case RelType::R_386_32:return "R_386_32";
        case RelType::R_386_PC32:return "R_386_PC32";
        default:return "ERROR";
    }
}
ostream& operator<<(ostream& os,const RelocationTable& rt){
    os<<"#.ret"<<rt.section<<endl;
    os<<"#offset\t#type\tvr["<<rt.section<<"]"<<endl;
    for(Relocation r:rt.relocations){
        os<<r<<endl;
    }
    os<<"#end";
    return os;
}
ostream& operator<<(ostream&os,const Relocation& r){
    
    os<<r.getHexOffset()<<"\t"<<r.getRelTypeText()<<"\t"<<r.s.number;
    return os;
}