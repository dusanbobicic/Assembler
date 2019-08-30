#ifndef SECTION_H
#define SECTION_H
#include <string>
#include <iostream>
#include <map>
#include <stdint.h>
#include "Enums.h"
using namespace std;
class Section{
    public:
    void addData(char val){
        data.insert_or_assign(counter,val);
        counter++;
    }
    void addMultipleData(int number,char val);
    void addTwoBytesData(short val);
    void addFourBytesData(int val);
    void changeData(int position,char val){
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