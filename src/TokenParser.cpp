#include "TokenParser.h"

const char *TokenParser::DELIMETERS = " ,\t";

TokenParser::TokenParser(ifstream &in)
{
    string line;
    while (getline(in, line))
    {
        //line.erase(line.length() - 1); //delete \n char
        vector<string> tokens;
		
        removeWhiteSpacesFromStart(line);
        if (line.length() == 0 || line.at(0) == '#' )
            continue;
        split(line, DELIMETERS, tokens);
        if (tokens.size() == 0)
        {
            continue; //ako nema tokena
        }
        //if comment

        //if end of assembly code
        if (tokens[0] == ".end")
        {
            inputAssemmbly.push_back(tokens);
            break;
        }
        inputAssemmbly.push_back(tokens);
    }
}
void TokenParser::split(string line, const char *delim, vector<string> &tokens)
{
    size_t start = line.find_first_not_of(delim);

    size_t end = start;

    while (start != std::string::npos)
    {
        end = line.find_first_of(delim, start);

        tokens.push_back(line.substr(start, end - start));

        start = line.find_first_not_of(delim, end);
    }
}
void TokenParser::removeWhiteSpacesFromStart(string &line)
{
    if (line.empty())
        return;
    if (line.at(0) == '\n')
        line.resize(0);
    if (line.at(0) == '\t' || line.at(0) == ' ')
    {
        int i = 0;
        while (i < line.length() && (line.at(i) == '\t' || line.at(i) == ' '))
            i++;
        line.erase(0, i);
    }
}
vector<vector<string>> &TokenParser::getAssembly()
{
    return this->inputAssemmbly;
}
TokenType TokenParser::parseToken(const string &token)
{
    using namespace std;
    string input=token;
    TokenType tt=INCORECT;
    regex label{ "([a-zA-Z][a-zA-Z0-9_]*):" };
	regex section{ "(\\.)(text|data|bss|section)" };
	regex directive{ "\\.(byte|word|align|skip|equ)" };
	regex instruction{ "(int|add|sub|mul|div|cmp|and|or|not|test|push|pop|call|iret|mov|shl|shr|ret|halt|jmp|jeq|jne|jgt|xchg)(b|w)?"};
	regex symbol{("^([a-zA-Z_][a-zA-Z0-9]*)$")};
	regex op_dec{ ("([0-9]+)") };
	regex op_sym_value{ "(\\&)([a-zA-Z_][a-zA-Z0-9]*)" };
	regex op_sym$_value{ "(\\$)([a-zA-Z_][a-zA-Z0-9]*)" };
	regex end{ "(\\.)(end)" };
    regex exp{"([a-zA-Z][a-zA-Z0-9_]*)+-"};
	if (!input.compare(".end")) {
		return TokenType::END;
	}
    if(!input.compare(".global")){
        return TokenType::GLB;
    }
    if(!input.compare(".extern")){
        return TokenType::EXT;
    }
    if(regex_match(input,label)){
        return TokenType::LABEL;
    }
    if(regex_match(input,section)){
        return TokenType::SECTION;
    }
    if(regex_match(input,directive)){
        return TokenType::DIRECTIVE;
    }
    if(regex_match(input,instruction)){
        return TokenType::INSTRUCTION;
    }
    if(regex_match(input,symbol)){
        return TokenType::SYMBOL;
    }
    if(regex_match(input,op_dec)){
        return TokenType::OP_DEC;
    }
     if(regex_match(input,op_sym_value) || regex_match(input, op_sym$_value)){
        return TokenType::OP_SYM_VALUE;
    }
    if(regex_match(input,exp)){
        return TokenType::EXP;
    }
    return tt;
}