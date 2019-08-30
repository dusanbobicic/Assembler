#include "Section.h"


void Section::addMultipleData(int number,char val){
    for(int i=0;i<number;i++){
        addData(val);
    }
}
void Section::addTwoBytesData(short val){
    //obrnutim redosledom jer je little endian
    char MASK=0xFF;
    
    unsigned char value=(unsigned char)val & MASK;//donji bajt
    addData(value);
    value=(val>>8) & MASK;//gornji bajt
    addData(value);
   
}
void Section::addFourBytesData(int val){
//obrnutim redosledom jer je little endian
    char MASK=0xFF;
    //first
    char value=(char)val & MASK;//donji bajt
    addData(value);
    //second
    value=(char)(val>>8) & MASK;//gornji bajt
    addData(value);
    //third
    value=(char)(val>>16) & MASK;//gornji bajt
    addData(value);
    //fourth
    value=(char)(val>>24) & MASK;//gornji bajt
    addData(value);
}
ostream& operator<<(ostream &os,const Section& s){
    os<<"#"<<s.name<<endl;
	char str[10];
    for(auto it:s.data){
        
        sprintf(str,"%02X",it.second);
        string out(str);
        os<<out<<"\t";
    }
	os << endl << "#end";
    return os;
}