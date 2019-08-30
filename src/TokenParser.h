#ifndef TOKENPARSER_H
#define TOKENPARSER_H
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>
#include "Enums.h"

using namespace std;
class TokenParser
{
public:
    TokenParser(ifstream &in);

    void split(string line, const char *delim, vector<string> &tokens);
    void removeWhiteSpacesFromStart(string &line);
    static TokenType parseToken(const string& parse);
    vector<vector<string>>& getAssembly();
    static const char *DELIMETERS;

private:
    vector<vector<string>> inputAssemmbly;
};


#endif