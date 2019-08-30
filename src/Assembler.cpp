#include "Assembler.h"
#include "TokenParser.h"

int Assembler::regBuilder = 0;
bool Assembler::currInsnmb = false;
map<string, int> Assembler::instructionNumOperands =
{
	{"add", 2},
	{"sub", 2},
	{"mul", 2},
	{"div", 2},
	{"cmp", 2},
	{"xchg", 2},
	{"and", 2},
	{"or", 2},
	{"xor", 2},
	{"not", 1},
	{"test", 2},
	{"push", 1},
	{"pop", 1},
	{"iret", 0},
	{"call", 1},
	{"mov", 2},
	{"shl", 2},
	{"shr", 2},
	{"ret", 0},
	{"jmp", 1},
	{"jeq", 1},
	{"jne", 1},
	{"jgt", 1},
	{"halt", 0},
	{"int", 1}

};

map<string, char> Assembler::instructionOpCode =
{
	{"halt", 1},
	{"xchg", 2},
	{"int", 3},
	{"mov", 4},
	{"add", 5},
	{"sub", 6},
	{"mul", 7},
	{"div", 8},
	{"cmp", 9},
	{"not", 10},
	{"and", 11},
	{"or", 12},
	{"xor", 13},
	{"test", 14},
	{"shl", 15},
	{"shr", 16},
	{"push", 17},
	{"pop", 18},
	{"jmp", 19},
	{"jeq", 20},
	{"jne", 21},
	{"jgt", 22},
	{"call", 23},
	{"ret", 24},
	{"iret", 25} };
Assembler::Assembler(string in, string out)
{
	this->in = in;
	this->out = out;
}

void Assembler::doMagic()
{
	ifstream ins;
	ofstream outs;
	ins.open(in);
	outs.open(out);
	cout << "Magic!" << endl;
	try {
		TokenParser tokenP(ins);

		assembly = tokenP.getAssembly();
		cout << "File " << in << " tokenized successfully!" << endl;
		prepareFirstPass();
		doFirstPass();
		cout << "Done first pass!" << endl;
		prepareSeecondPass();
		doSecondPass();
		cout << "Done second pass!" << endl;
		writeOut(outs);
		cout << "Object file generated successfully at" << out << "!" << endl;
	}
	catch (runtime_error re) {
		outs << re.what() << endl;
		cout << "Errors in code! " << re.what() << endl;
	}
	catch (exception e) {
		outs << e.what() << endl;
		cout << "Internal exception happened: " << e.what() << endl;
	}
	if (outs.is_open()) {

		outs.close();
	}
	if (ins.is_open()) {
		ins.close();
	}
}
void Assembler::writeOut(ofstream& os) {
	os << symbolTable << endl;
	os << endl;
	for (auto it : relTables) {
		os << it.second << endl;
		os << endl;
	}
	for (auto it : sections) {
		os << it.second << endl;
		os << endl;
	}
	os.flush();
}
void Assembler::doFirstPass()
{
	string currToken;

	//add new undeefined symbol
	for (const auto& tokens : assembly)
	{
		queue<string> tokenStream;
		for (const auto& token : tokens)
		{
			tokenStream.push(token);
		}
		currToken = tokenStream.front();
		TokenType tt = TokenParser::parseToken(currToken);
		if (tt == TokenType::LABEL)
		{
			if (currentSection == SectionLabel::START)
			{
				throw runtime_error("Label cannot be defined in start section!"); //exterapolate string to const
			}
			string labelName = currToken.substr(0, currToken.size() - 1); //trim the ':'
			tokenStream.pop();

			Symbol* p = new Symbol(labelName, currentSectionS, currentSection, locationCounter, Scope::LOCAL, TokenType::LABEL, 0);
			p->defined = true;
			symbolTable.addNewSymbol(*p);

			if (tokenStream.empty())
			{
				continue;
				//if there is nothing after the label
			}
		}
		currToken = tokenStream.front();
		tt = TokenParser::parseToken(currToken); //ako smo imali labelu pa instrukciju recimo
		//ispalimo token
		tokenStream.pop();
		switch (tt)
		{
		case TokenType::LABEL:
			throw runtime_error("No double labels allowed in one row!"); //exterapolate string to const
			break;
		case TokenType::EXT:
			if (currentSection != START)
			{
				throw runtime_error("Global cannot be defined out of the section!");
			}
			while (!tokenStream.empty())
			{
				string sy = tokenStream.front();
				TokenType tts = TokenParser::parseToken(sy);
				if (tts != SYMBOL)
				{
					throw runtime_error("After extern must be symbol!");
				}
				Symbol* p = new Symbol(sy, "und", SectionLabel::UND, locationCounter, Scope::GLOBAL, TokenType::LABEL, 0);
				symbolTable.addNewSymbol(*p);

				tokenStream.pop();
			}
			break;
		case TokenType::GLB:
			if (currentSection != START)
			{
				throw runtime_error("Global cannot be defined out of the START section!");
			}
			while (!tokenStream.empty())
			{
				tokenStream.pop(); //ukoilko savo vaj kod ubacimo u neki for ili while, onda
				//ispalimo sve posle .global-a
				//nisam obradjivao global ovde jer sta ako se labela ne nalazi nigde :(
				//onda je to teze proveriti
			}
			break;
		case TokenType::DIRECTIVE:
			if (currentSection == START || currentSection == TEXT)
			{
				throw runtime_error("Directives can't be defined in START or TEXT section!");
			}
			parseDirectiveFRun(currToken, tokenStream);
			break;
		case TokenType::SECTION: {
			int flags = 0;
			if (currentSection != SectionLabel::START)
			{
				if (currentSectionS == ".section") {
					string name = tokenStream.front();
					regex sec_name{ "([a-zA-Z][a-zA-Z0-9_]*)" };
					smatch sm;
					if (regex_match(name, sm, sec_name)) {
						name = sm[0];
						cout << "Found custom section " << name << endl;
						currentSectionS = name;
					}
				}
				Section* sec = new Section();
				sec->name = currentSectionS;
				sec->size = locationCounter;
				sections.insert_or_assign(currentSectionS, *sec);
				symbolTable.symbols.find(currentSectionS)->second.size = locationCounter;
			}
			currentSectionS = currToken;

			if (currentSectionS == ".section") {
				string name = tokenStream.front();
				tokenStream.pop();
				regex sec_name{ "([a-zA-Z][a-zA-Z0-9_]*)" };
				smatch sm;
				if (regex_match(name, sm, sec_name)) {
					name = sm[0];
					cout << "Found custom section " << name << endl;
					currentSectionS = name;
				}
				if (!tokenStream.empty()) {
					string t = tokenStream.front();
					tokenStream.pop();
					flags = getFlagValFromText(t);

				}
				else {
					flags = 0x04;
				}

			}
			else {
				flags = getFlagValue(currentSectionS);
			}
			updateDataSection(currentSectionS, currentSection);
			Symbol* sym = new Symbol(currentSectionS, currentSectionS, currentSection, locationCounter, Scope::LOCAL, TokenType::SECTION, 0);
			sym->flag = flags;
			symbolTable.addNewSymbol(*sym);
			break;
		}

		case INSTRUCTION:
			if (currentSection != TEXT)
				throw runtime_error("Instrucitons can't be defined outside of TEXT section!");
			instructionFirstRun(currToken, tokenStream);
			break;
		case END:
			if (currentSection != SectionLabel::START)
			{
				Section* s = new Section();
				s->name = currentSectionS;
				s->size = locationCounter;
				sections.insert_or_assign(currentSectionS, *s);
				symbolTable.symbols.find(currentSectionS)->second.size = locationCounter;
			}
			break;
		default:
			throw runtime_error("Wrong token at the start of line during first pass! Token: " + currToken);
			break;
		}
	}
	tryParseAllEqu();
}
void Assembler::prepareFirstPass()
{
	locationCounter = 0;
	currentSection = SectionLabel::START;
	currentSectionS = "start";
}
void Assembler::prepareSeecondPass()
{
	locationCounter = 0;
	currentSection = SectionLabel::START;
	currentSectionS = "start";
}
void Assembler::doSecondPass()
{
	//DoSecondPass
	TokenType tt;
	string currToken;
	for (const auto& tokens : assembly)
	{
		queue<string> tokenStream;
		for (const auto& token : tokens)
		{
			tokenStream.push(token);
		}
		currToken = tokenStream.front();
		tt = TokenParser::parseToken(currToken);
		if (tt == TokenType::LABEL)
		{
			tokenStream.pop();
			if (tokenStream.empty())
			{
				continue;
				//if there is nothing after the label
			}
		}
		currToken = tokenStream.front();
		tt = TokenParser::parseToken(currToken); //ako smo imali labelu pa instrukciju recimo
		//ispalimo token
		tokenStream.pop();
		switch (tt)
		{
		case TokenType::EXT:

			while (!tokenStream.empty())
			{

				tokenStream.pop();
			}
			break;
		case TokenType::GLB:
			while (!tokenStream.empty())
			{
				string currentTokenS = tokenStream.front();
				tokenStream.pop();
				TokenType currTok = TokenParser::parseToken(currentSectionS);
				auto it = symbolTable.symbols.find(currentTokenS);
				if (it != symbolTable.symbols.end())
				{
					it->second.sc = Scope::GLOBAL;
				}
			}
			break;
		case TokenType::DIRECTIVE:
			if (currentSection == START || currentSection == TEXT)
			{
				throw runtime_error("Directives can't be defined in START or TEXT section!");
			}
			parseDirectiveSRun(currToken, tokenStream); //implement
			break;
		case TokenType::SECTION:
			currentSectionS = currToken;
			if (currentSectionS == ".section") {
				string name = tokenStream.front();
				regex sec_name{ "([a-zA-Z][a-zA-Z0-9_]*)" };
				smatch sm;
				if (regex_match(name, sm, sec_name)) {
					name = sm[0];
					cout << "Found custom section " << name << endl;
					currentSectionS = name;
				}
			}
			updateDataSection(currentSectionS, currentSection);

			break;
		case INSTRUCTION:
			instructionSecondRun(currToken, tokenStream);
			break;

		}
	}
}

//Helping functions
void Assembler::parseDirectiveFRun(string directive, queue<string> ts)
{
	if (directive == ".align")
	{
		string op = ts.front();
		ts.pop();
		TokenType tt = TokenParser::parseToken(op);
		if (tt != TokenType::OP_DEC)
			throw runtime_error("Operand must be a number!");
		int value = getValueOperand(op, currentSectionS);
		if (!(locationCounter % value == 0))
		{
			locationCounter = locationCounter / value * value + value;
		}
		return;
	}
	if (directive == ".skip")
	{
		string op = ts.front();
		ts.pop();
		TokenType opT = TokenParser::parseToken(op);
		if (opT != TokenType::OP_DEC)
			throw runtime_error("Directive skip need decimal operand .");
		int value = getValueOperand(op, currentSectionS);
		locationCounter += value;
		return;
	}
	int pom = 0;
	if (directive == ".word")
	{
		pom = NumOperandsDirective(ts);
		locationCounter += 2 * pom;
		return;
	}
	if (directive == ".byte")
	{
		pom = NumOperandsDirective(ts);
		locationCounter += 1 * pom;
		return;
	}
	if (directive == ".equ")
	{
		processEqu(ts);
		return;
	}
}
void Assembler::parseDirectiveSRun(string directive, queue<string> ts)
{
	auto iter = symbolTable.symbols.find(currentSectionS);
	if (iter == symbolTable.symbols.end())throw runtime_error("Section symbol not inserted!");

	Symbol& s = iter->second;
	if (directive != ".align" && directive != ".skip" && currentSectionS == ".bss")
	{
		throw runtime_error("Directive .long and .word can not be defined in .bss section");
	}
	if (directive == ".align")
	{
		if (!(s.flag & 0x04))throw runtime_error("Can't read from this section!");
		int oldCounter = locationCounter;
		string opS = ts.front();
		ts.pop();
		TokenType tt = TokenParser::parseToken(opS);
		int val = getValueOperand(opS, currentSectionS, currentSection);
		if (!(locationCounter % val == 0))
		{
			locationCounter = locationCounter / val * val + val; //poravnanje
		}
		auto sec = sections.find(currentSectionS);
		sec->second.addMultipleData(locationCounter - oldCounter, 0);

		return;
	}
	if (directive == ".skip")
	{
		if (!(s.flag & 0x04))throw runtime_error("Can't read from this section!");
		string opS = ts.front();
		ts.pop();
		TokenType tt = TokenParser::parseToken(opS);
		if (tt != TokenType::OP_DEC)
			throw runtime_error("Invalid operand for .skip xD");
		char val = getValueOperand(opS, currentSectionS, currentSection);

		auto sec = sections.find(currentSectionS);
		if (sec == sections.end())
			throw runtime_error("Skip not in section xD");
		sec->second.addMultipleData(val, 0);
		locationCounter += val;
		return;
	}
	if (directive == ".byte")
	{
		if (!(s.flag & 0x02))throw runtime_error("Can't write into this section!");
		while (!ts.empty())
		{
			string opS = ts.front();
			ts.pop();
			TokenType tt = TokenParser::parseToken(opS);
			char val = getValueOperand(opS, currentSectionS, currentSection);

			auto sec = sections.find(currentSectionS);
			if (sec == sections.end())
				throw runtime_error("byte not in section xD");
			sec->second.addData(val);
			locationCounter++;
		}
		return;
	}
	if (directive == ".word")
	{
		if (!(s.flag & 0x02))throw runtime_error("Can't write into this section!");
		while (!ts.empty())
		{
			string opS = ts.front();
			ts.pop();
			TokenType tt = TokenParser::parseToken(opS);
			int val = getValueOperand(opS, currentSectionS, currentSection);

			auto sec = sections.find(currentSectionS);
			if (sec == sections.end())
				throw runtime_error("word not in section xD");
			sec->second.addTwoBytesData(val);
			locationCounter += 2;
		}
		return;
	}
	//Uncomment for LONG support;
	/*if(directive==".long"){
		while (!ts.empty())
		{
			string opS = ts.front();
			ts.pop();
			TokenType tt = TokenParser::parseToken(opS);
			if (tt != TokenType::OP_DEC)
				throw runtime_error("Invalid operand for .byte xD");
			unsigned int val = getValueOperand(opS, currentSectionS, currentSection);

			auto sec = sections.find(currentSectionS);
			if (sec == sections.end())
				throw runtime_error("byte not in section xD");
			sec->second.addFourBytesData(val);
			locationCounter+=4;
		}
		return;
	}*/

	//---
}
void Assembler::processEqu(queue<string> ts)
{
	string tok = ts.front();
	ts.pop();
	if (currentSection == SectionLabel::TEXT)
	{
		throw runtime_error("Invalid section for equ!");
	}
	TokenType tt = TokenParser::parseToken(tok);
	if (tt != TokenType::SYMBOL)
	{
		throw runtime_error("Invalid symbol for equ!");
	}
	Symbol* s = new Symbol(tok, currentSectionS, currentSection, locationCounter, Scope::LOCAL, TokenType::LABEL, 0);
	symbolTable.addNewSymbol(*s);
	regex symb{ ("^([a-zA-Z_][a-zA-Z0-9]*)$") };
	while (!ts.empty())
	{
		string val = ts.front();
		ts.pop();
		s->expression.push_back(val);
	}
}
void Assembler::tryParseAllEqu()
{
	for (auto& iter : symbolTable.symbols)
	{
		Symbol& sym = iter.second;
		if (sym.isEqu)
		{
			bool flg = sym.defined = tryParseSymbolEqu(sym);
			sym.isEqu = !flg; //ako fja vrati false, equ ostaje, ako vrati true nije vise equ
			//to znaci da je uspesno parsirala
		}
	}
}
bool Assembler::tryParseSymbolEqu(Symbol& s)
{
	regex op_sym{ "^([a-zA-Z_][a-zA-Z0-9]*)$" };
	regex oper{ "\\B[+-]\\B" };
	int value = 0;
	int mul = 1;
	for (const auto& iter : s.expression)
	{
		//preskace znakove gleda smao simbole
		if (regex_match(iter, op_sym))
		{
			auto it = symbolTable.symbols.find(iter);
			if (!it->second.defined)
			{
				return false;
			}
		}
	}
	for (const auto& iter : s.expression)
	{
		cout << "Parsing equ expression for " << s.name << ": " << iter;
		if (regex_match(iter, op_sym))
		{
			auto it = symbolTable.symbols.find(iter);
			//ako posotji taj simbol u tabeli simbola
			if (it != symbolTable.symbols.end())
			{
				Symbol& sym = it->second;
				value += mul * sym.offset;
			}
			else
			{
				throw runtime_error("Symbol " + it->second.name + " does not exist!");
			}
		}

		else if (iter.compare("+"))
		{
			mul *= 1;
		}
		else if (iter.compare("-"))
		{
			mul *= -1;
		}
		else
		{
			cout << "Greska u tokenu " << iter;
		}
	}
	return true;
}
void Assembler::instructionFirstRun(string instr, queue<string>& tokens)
{
	smatch match;
	std::regex instruction{ "(int|add|sub|mul|div|cmp|and|or|not|test|push|pop|call|iret|mov|shl|shr|ret|halt|jmp|jeq|jne|jgt|xchg)(b|w)?" };

	std::regex_match(instr, match, instruction);
	string icode = match[1];
	int size = instructionNumOperands[icode];
	int largeinstr = false;

	if (size == 0)
	{
		locationCounter += 1; //velicina instrukcije
		return;
	}

	string op1 = tokens.front();
	int adrType = resolveAddresingTypeOperand(op1);
	tokens.pop();
	switch (adrType)
	{
	case AddressingType::IMM:
		throw runtime_error("Neposredna vrednsot je invalidni operand!");
	case AddressingType::REG_IND:
	case AddressingType::REG_DIR:
		locationCounter += 1;
		break;
	case AddressingType::REG_IND_POM8:
		locationCounter += 2;
		break;
	case AddressingType::REG_IND_POM16:
	case AddressingType::PC_POM:
	case AddressingType::MEM:
		locationCounter += 3;
		break;
	default:
		throw runtime_error("Lose adresiranje!");
		break;
	}
	if (size == 1)
	{
		locationCounter += 1;
		return;
	}
	string op2 = tokens.front();
	tokens.pop();
	adrType = resolveAddresingTypeOperand(op2);
	//op2
	switch (adrType)
	{

	case AddressingType::REG_IND:
	case AddressingType::REG_DIR:
		locationCounter += 1;
		break;
	case AddressingType::REG_IND_POM8:
		locationCounter += 2;
		break;
	case AddressingType::REG_IND_POM16:
	case AddressingType::PC_POM:
	case AddressingType::MEM:
	case AddressingType::IMM:

		locationCounter += 3; //+1 jer imamo sam bajt adresiranja
		break;
	default:
		throw runtime_error("Lose adresiranje!");
		break;
	}
	if (!tokens.empty())
	{

		throw runtime_error("More tokens left! Ivalid!");
	}
	locationCounter += 1; //+1 za kod oepracije
}
void Assembler::instructionSecondRun(string instr, queue<string>& ts) {

	smatch match;
	std::regex instruction{ "(int|add|sub|mul|div|cmp|and|or|not|test|push|pop|call|iret|mov|shl|shr|ret|halt|jmp|jeq|jne|jgt|xchg)(b|w)?" };
	/*
		1       2       3   4   5       6   7
		insDe   op1desc add add op2dec  add add
	*/
	std::regex_match(instr, match, instruction);
	string icode = match[1];
	char DATA1 = 0;
	int size = instructionNumOperands[icode];
	auto ins = instructionOpCode.find(icode);
	char opCode = ins->second;
	char MASK = 0xFF;
	DATA1 = (opCode << 3) & MASK;//da ga stavimo na bite od 3 do 7
	//dodati support za b i w
	auto sec = sections.find(currentSectionS);
	sec->second.addData(DATA1);//dodajemo OPCODE u sadrzaj sekcije
	locationCounter++;//idemo dalje
	switch (size) {
	case 1:
		handleInsOneOperand(instr, ts, sec->second);
		break;
	case 2:
		handleInsTwoOperand(instr, ts, sec->second);
		break;
	case 0:
		handleInsZerOperand(instr, ts, sec->second);
		break;
	}

}
void Assembler::handleInsOneOperand(string instruction, queue<string> ts, Section& s) {
	string op1 = ts.front();
	ts.pop();
	AddressingType addT = resolveAddresingTypeOperand(op1);
	bool nmb;
	int reg = 0;
	int opcode = instructionOpCode[instruction];
	if (addT == AddressingType::IMM)throw runtime_error("Imm cant ve destination");
	locationCounter += 1;//+1 zbog opisa operanda
	int value = getValueOperand(op1, currentSectionS, currentSection, nmb, reg);
	locationCounter -= 1;//jos jedan zbog opisa operanda
	char DATA1;
	int addressOffset = 5;
	short insert;
	char regs;
	switch (addT) {


	case AddressingType::REG_DIR:
	case AddressingType::REG_IND:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto
		s.addData(DATA1);
		locationCounter += 1;
		break;
	case AddressingType::REG_IND_POM8:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto

		locationCounter += 2;
		s.addData(DATA1);
		DATA1 = (value & 0x00FF);
		s.addData(DATA1);
		break;
	case AddressingType::PC_POM:
		addT = REG_IND_POM16;
	case AddressingType::REG_IND_POM16:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto

		locationCounter += 3;
		s.addData(DATA1);
		insert = (value & 0x0FFFF);
		s.addTwoBytesData(insert);
		break;
	case AddressingType::MEM:
		DATA1 = (addT << addressOffset);//formiram novi bajt tako sto
		locationCounter += 3;
		s.addData(DATA1);
		insert = (value & 0x0FFFF);
		s.addTwoBytesData(insert);
		break;
	}


}
void Assembler::handleInsTwoOperand(string instruction, queue<string> ts, Section& s) {
	string op1 = ts.front();
	ts.pop();
	AddressingType addT = resolveAddresingTypeOperand(op1);
	bool nmb;
	int reg = 0;
	int opcode = instructionOpCode[instruction];
	if (addT == AddressingType::IMM)throw runtime_error("Imm cant be destination");
	locationCounter += 1;
	int value = getValueOperand(op1, currentSectionS, currentSection, nmb, reg);
	locationCounter -= 1;
	char DATA1;
	int addressOffset = 5;
	short insert;
	char regs;
	switch (addT) {

	case AddressingType::REG_DIR:
	case AddressingType::REG_IND:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto
		s.addData(DATA1);
		locationCounter += 1;
		break;
	case AddressingType::REG_IND_POM8:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto

		locationCounter += 2;
		s.addData(DATA1);
		DATA1 = (value & 0x00FF);
		s.addData(DATA1);
		break;
	case AddressingType::PC_POM:
		addT = REG_IND_POM16;
	case AddressingType::REG_IND_POM16:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto

		locationCounter += 3;
		s.addData(DATA1);
		insert = (value & 0x0FFFF);
		s.addTwoBytesData(insert);
		break;
	case AddressingType::MEM:
		DATA1 = (addT << addressOffset);//formiram novi bajt tako sto
		locationCounter += 3;
		s.addData(DATA1);
		insert = (value & 0x0FFFF);
		s.addTwoBytesData(insert);
		break;
	}
	string op2 = ts.front();
	addT = resolveAddresingTypeOperand(op2);
	ts.pop();
	locationCounter += 1;
	value = getValueOperand(op2, currentSectionS, currentSection, nmb, reg);
	locationCounter -= 1;
	switch (addT) {


	case AddressingType::REG_DIR:
	case AddressingType::REG_IND:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto
		s.addData(DATA1);
		locationCounter += 1;
		break;
	case AddressingType::REG_IND_POM8:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto

		locationCounter += 2;
		s.addData(DATA1);
		DATA1 = (value & 0x00FF);
		s.addData(DATA1);
		break;
	case AddressingType::PC_POM:
		addT = REG_IND_POM16;
	case AddressingType::REG_IND_POM16:
		regs = reg << 1;
		DATA1 = (addT << addressOffset) | regs;//formiram novi bajt tako sto

		locationCounter += 3;
		s.addData(DATA1);
		insert = (value & 0x0FFFF);
		s.addTwoBytesData(insert);
		break;
	case AddressingType::MEM:
	case AddressingType::IMM:
		DATA1 = (addT << addressOffset);//formiram novi bajt tako sto
		locationCounter += 3;
		s.addData(DATA1);
		insert = (value & 0x0FFFF);
		s.addTwoBytesData(insert);
		break;
	}



}
void Assembler::handleInsZerOperand(string instruction, queue<string> ts, Section& s) {

}
int Assembler::getValueOperand(string operand, string css, SectionLabel cs, bool& nmb, int& reg)
{
	std::regex op_dec{ ("([0-9]+)") };
	std::regex op_sym_value{ "(\\&)([a-zA-Z_][a-zA-Z0-9]*)" };
	std::regex op_sym_mem{ "([a-zA-Z_][a-zA-Z0-9]*)" };
	std::regex op_mem{ "(\\*)([0-9]+)" };
	std::regex op_reg{ "(r)([0-7])" };
	std::regex op_reg_ind_value8{ "(r[0-7])(\\[)([0-9]{2})(\\])" };
	std::regex op_reg_ind_value16{ "(r[0-7])(\\[)([0-9]{4})(\\])" };
	std::regex op_reg_ind_sym{ "(r)([0-7])(\\[)([a-zA-Z_][a-zA-Z0-9]*)(\\])" };
	std::regex op_$_label{ "(\\$)([a-zA-Z_][a-zA-Z0-9]*)" };

	smatch match;
	if (regex_match(operand, match, op_dec))
	{
		nmb = true; //Ovo je decimalna vrednost samo zadata.
		return stoi(match[1]);
	}
	if (regex_match(operand, match, op_reg))
	{
		nmb = false;
		string rNumber = match[2];
		int pom = stoi(rNumber);

		reg = pom;
		return 0;
	}
	if (regex_match(operand, match, op_sym_value))
	{
		nmb = true;
		string symS = match[2]; //Ovo je simbol I ovo moze da se javi u direktivama.
		return setAbsRelocation(symS, css);
	}
	if (regex_match(operand, match, op_sym_mem))
	{
		nmb = true;
		string symS = operand;
		return setAbsRelocation(symS, css);
	}
	if (regex_match(operand, match, op_mem))
	{
		nmb = true;
		return stoi(match[2]);
	}

	if (regex_match(operand, match, op_reg_ind_value8))
	{
		nmb = true;
		string rNumber = match[2]; //Zbog ovih zagrada je na 3. poziciji.
		reg = stoi(rNumber);
		return stoi(match[4]);//vracamo pomeraj
	}
	if (regex_match(operand, match, op_reg_ind_sym))
	{
		nmb = true;
		string rNumber = match[2]; //Zbog ovih zagrada je na 3 poziciji.
		reg = stoi(rNumber);
		string symS = match[4];
		return setAbsRelocation(symS, currentSectionS);
	}
	if (regex_match(operand, match, op_$_label))
	{
		/*if(flagJMP=false){
			string rNumber="7";
			reg=decToBinString(rNumber);
		}
		*/
		nmb = true;
		string rNumber = "7";
		reg = stoi(rNumber);
		string symS = match[2];
		return setPCRelRelocation(symS, currentSectionS);
	}
	throw runtime_error("Unknown operand!");
}
int Assembler::setAbsRelocation(string symb, string currSecs)
{
	if (relTables.find(currSecs) == relTables.end())
	{
		//no table initialized for tihs section
		RelocationTable* rt = new RelocationTable(currSecs);
		rt->section = currSecs;
		relTables.insert_or_assign(currSecs, *rt);
	}
	auto table = relTables.find(currSecs);
	RelocationTable& relT = table->second;
	auto symbol = symbolTable.symbols.find(symb);
	if (symbol == symbolTable.symbols.end())
		throw runtime_error("Symbol nije definisan!");

	if (symbol->second.sc == Scope::GLOBAL)
	{
		Relocation* r = new Relocation(locationCounter, RelType::R_386_32, symbol->second);
		//ako je globalni simbol, referenciramo bas taj simbol.

		relT.relocations.push_back(*r);
		return 0;
	}
	else
	{
		string sec = symbol->second.secs;
		auto symb = symbolTable.symbols.find(sec)->second;
		Relocation* r = new Relocation(locationCounter, RelType::R_386_32, symb);
		//ako je lokalni simbool, referencira se sekcija u kojoj pripada
		relT.relocations.push_back(*r);
		return symbol->second.offset;
	}
}
int Assembler::setPCRelRelocation(string symS, string currSecs) {
	auto& symbol = symbolTable.symbols.find(symS)->second;
	if (relTables.find(currSecs) == relTables.end())
	{
		//no table initialized for tihs section
		RelocationTable* rt = new RelocationTable(currSecs);
		rt->section = currSecs;
		relTables.insert_or_assign(currSecs, *rt);
	}
	auto tbl = relTables.find(currSecs);
	if (symbol.sc == Scope::GLOBAL) {
		Relocation* r = new Relocation(locationCounter, RelType::R_386_PC32, symbol);
		tbl->second.relocations.push_back(*r);
		return -2;//upisujem jer imamo globalni simbol
	}
	else {

		string sec = symbol.secs;
		auto symb = symbolTable.symbols.find(sec)->second;
		Relocation* r = new Relocation(locationCounter, RelType::R_386_32, symb);
		tbl->second.relocations.push_back(*r);
		return symbol.offset - 2;//offset simbola od pocetka sekcije minu 2 jer imamo 2 bajta
	}
}
int Assembler::NumOperandsDirective(queue<string>& tokens)
{
	int ret = 0;
	while (!tokens.empty())
	{
		tokens.pop();
		ret++;
	}
	return ret;
}
void Assembler::updateDataSection(string cts, SectionLabel& cs)
{
	locationCounter = 0;
	if (cts == ".text")
	{
		cs = SectionLabel::TEXT;
		return;
	}
	if (cts == ".data")
	{
		cs = SectionLabel::DATA;
		return;
	}
	if (cts == ".section")
	{
		cs = SectionLabel::OTHER;
		return;
	}
	if (cts == ".bss")
	{
		cs = SectionLabel::BSS;
		return;
	}
}
AddressingType Assembler::resolveAddresingTypeOperand(string firstT)
{
	std::regex op_dec{ ("^([0-9]{2})$") };
	std::regex op_dec8{ ("^([0-9]{2})$") };
	std::regex op_sym_value{ "(\\&)([a-zA-Z_][a-zA-Z0-9]*)" };
	std::regex op_sym_mem{ "([a-zA-Z_][a-zA-Z0-9]*)" };
	std::regex op_mem{ "(\\*)([0-9]+)" };
	std::regex op_reg{ "r[0-7]" };
	std::regex op_reg_ind_value8{ "(r[0-7])(\\[)([0-9]{2})(\\])" };
	std::regex op_reg_ind_value16{ "(r[0-7])(\\[)([0-9]{4})(\\])" };
	std::regex op_reg_ind_sym{ "(r[0-7])(\\[)([a-zA-Z_][a-zA-Z0-9]*)(\\])" };
	std::regex op_$_label{ "(\\$)([a-zA-Z_][a-zA-Z0-9]*)" };

	if (regex_match(firstT, op_dec))
	{
		return AddressingType::IMM;
	}
	if (regex_match(firstT, op_reg))
	{
		return AddressingType::REG_DIR;
	}
	if (regex_match(firstT, op_sym_value))
	{
		return AddressingType::IMM;
	}
	if (regex_match(firstT, op_sym_mem))
	{
		return AddressingType::MEM;
	}
	if (regex_match(firstT, op_mem))
	{
		return AddressingType::MEM;
	}
	if (regex_match(firstT, op_reg_ind_value8))
	{
		return AddressingType::REG_IND_POM8;
	}
	if (regex_match(firstT, op_reg_ind_value16))
	{
		return AddressingType::REG_IND_POM16;
	}
	if (regex_match(firstT, op_reg_ind_sym))
	{
		return AddressingType::REG_IND_POM16;
	}
	if (regex_match(firstT, op_$_label))
	{
		return AddressingType::PC_POM;
	}
	throw runtime_error("Addressing type is not recognized");
}
int Assembler::getFlagValue(string css) {
	if (css == ".text") {
		return 0x05;
	}
	else if (css == ".data") {
		return 0x06;
	}
	else if (css == ".bss") {
		return 0x04;
	}
	else {
		throw runtime_error("Invalid section!");
	}
}
int Assembler::getFlagValFromText(string ind) {
	unsigned int val = 0;
	std::regex read{ "[R]" };
	std::regex write{ "[W]" };
	std::regex execute{ "[X]" };
	if (std::regex_search(ind, read)) {
		val |= 0x04;
	}
	if (std::regex_search(ind, write)) {
		val |= 0x02;
	}
	if (std::regex_search(ind, execute)) {
		val |= 0x01;
	}
	return val;

}