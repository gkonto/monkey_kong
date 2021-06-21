#include "symbols.hpp"
#include "token.hpp"
#include <iostream>

TokenType SymbolTable::LookupIdent(const std::string &key)
{
    SymbolTable &instance = getInstance();
    auto val = instance.keywords.find(key);

    if (val != instance.keywords.end())
    {
        return val->second;
    }

    return T_IDENT;
}

void SymbolTable::initializeSymbols()
{
    keywords.emplace("fn", T_FUNCTION);
    keywords.emplace("let", T_LET);
    keywords.emplace("true", T_TRUE);
    keywords.emplace("false", T_FALSE);
    keywords.emplace("if", T_IF);
    keywords.emplace("else", T_ELSE);
    keywords.emplace("return", T_RETURN);
}

SymbolTable &SymbolTable::getInstance()
{
    static SymbolTable s_table;
    return s_table;
}
