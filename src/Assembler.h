#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <regex>
#include <map>
#include <stdint.h>
#include "Symbol.h"
#include "Section.h"
#include "SymbolTable.h"
#include "RelocationTable.h"
#include "Enums.h"

using namespace std;
class Assembler{
public:
   

    Assembler(string in, string out);
    void doMagic();
private:

    void prepareFirstPass();
    void doFirstPass();
    void prepareSeecondPass();
    void doSecondPass();
    void writeOut(ofstream& os);

    void parseDirectiveFRun(string directive,queue<string> ts);
    void parseDirectiveSRun(string directive,queue<string> ts);

    void handleInsZerOperand(string instruction,queue<string> ts,Section &s);
    void handleInsOneOperand(string instruction,queue<string> ts,Section &s);
    void handleInsTwoOperand(string instruction,queue<string> ts,Section &s);

	int getFlagValue(string css);
	int getFlagValFromText(string ind);

    void updateDataSection(string cts, SectionLabel& cs);
    int getValueOperand(string operand,string css="",SectionLabel cs=SectionLabel::START,bool& nmb=currInsnmb,int& reg=regBuilder);
    int setAbsRelocation(string symb,string currSecs);
    int setPCRelRelocation(string symS,string currSecs);
    int NumOperandsDirective(queue<string> &tokens);
    void processEqu(queue<string> ts);
    void tryParseAllEqu();
    bool tryParseSymbolEqu(Symbol& s);
    void calcValueEqu();
    void instructionFirstRun(string instr, queue<string> &tokens);
    void instructionSecondRun(string instr,queue<string> &ts);
    AddressingType resolveAddresingTypeOperand(string firstT);
    
    
    string in,out;
    SectionLabel currentSection;
    string currentSectionS;
    string customSection;
    static int regBuilder;
    static bool currInsnmb;
    int locationCounter=0;
    vector<vector<string>> assembly;
    map<string,Section&> sections;
    map<string,RelocationTable> relTables;
    SymbolTable symbolTable;

    static map<string,int> instructionNumOperands;
    static map<string,char> instructionOpCode;

};

#endif