#include "SymbolTable.h"

SymbolTable::SymbolTable(){
    counter=0;

}
bool SymbolTable::addNewSymbol(Symbol &s)
    {
        if (symbols.find(s.name) != symbols.end())
        {
            s.number=counter;
            counter++;
            symbols.insert_or_assign(s.name, s);
            return true;
        }else{
            return false;
        }
    }
ostream& operator<<(ostream& os,const SymbolTable& st){
    os<<"#id\tname\tsection\ttype\toffset\tscope\tsize\tRWX"<<endl;
    for(auto const& sym:st.symbols){
        os<<sym.second<<endl;

    }
return os;
}