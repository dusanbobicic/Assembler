#include "Section.h"


void Section::addMultipleData(int number,unsigned char val){
    for(int i=0;i<number;i++){
        addData(val);
    }
}
void Section::addTwoBytesData(u_short val){
    //obrnutim redosledom jer je little endian
    u_char MASK=0xFF;
    
    u_char value=(u_char)val & MASK;//donji bajt
    addData(value);
    value=(val>>8) & MASK;//gornji bajt
    addData(value);
   
}
void Section::addFourBytesData(u_int32_t val){
//obrnutim redosledom jer je little endian
    u_char MASK=0xFF;
    //first
    u_char value=(u_char)val & MASK;//donji bajt
    addData(value);
    //second
    value=(u_char)(val>>8) & MASK;//gornji bajt
    addData(value);
    //third
    value=(u_char)(val>>16) & MASK;//gornji bajt
    addData(value);
    //fourth
    value=(u_char)(val>>24) & MASK;//gornji bajt
    addData(value);
}
ostream& operator<<(ostream &os,const Section& s){
    os<<"#"<<s.name<<endl;
    for(auto it:s.data){
        char str[5];
        sprintf(str,"%02X",it.second);
        string s(str);
        os<<s<<"\t";
    }
    
    return os;
}