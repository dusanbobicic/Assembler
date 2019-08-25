#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "Assembler.h"

using namespace std;
int main(int argc, char **argv)
{
    if(argc!=3){
        cout<<"Wrong number of operands!"<<endl;
        exit(-1);
    }
    string input, output;
    input=argv[1];//# dodati konstante
    output=argv[2];//# dodati konstante
    Assembler::Assembler magician(input,output);
    magician.doMagic();


    return 0;
}