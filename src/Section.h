#ifndef SECTION_H
#define SECTION_H
#include <string>
#include <iostream>
#include <map>
#include "Enums.h"
using namespace std;
class Section{
    public:
    void addData(unsigned char val){
        data.insert_or_assign(counter,val);
        counter++;
    }
    void addMultipleData(int number,unsigned char val);
    void addTwoBytesData(u_short val);
    void addFourBytesData(u_int32_t val);
    void changeData(int position,unsigned char val){
        data.insert_or_assign(position,val);
    }
    string name;
    SectionLabel sl;
    int size;
    int counter=0;
    map<int,unsigned char> data;
    friend ostream& operator<<(ostream &os,const Section& s);
    
};


#endif