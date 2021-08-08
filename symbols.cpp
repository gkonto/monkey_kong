#include "symbols.hpp"
#include "token.hpp"
#include <iostream>

//#define USE_MAP_ST

TokenType LookupIdentifier(const std::string &key)
{
#ifdef USE_MAP_ST
    return SymbolTableMap::LookupIdent(key);
#else
    return SymbolTableVec::LookupIdent(key);
#endif
}

TokenType SymbolTableMap::LookupIdent(const std::string &key)
{
    SymbolTableMap &instance = getInstance();
    auto val = instance.keywords.find(key);

    if (val != instance.keywords.end())
    {
        return val->second;
    }

    return T_IDENT;
}

void SymbolTableMap::initializeSymbols()
{
    keywords.emplace("fn", T_FUNCTION);
    keywords.emplace("let", T_LET);
    keywords.emplace("true", T_TRUE);
    keywords.emplace("false", T_FALSE);
    keywords.emplace("if", T_IF);
    keywords.emplace("else", T_ELSE);
    keywords.emplace("return", T_RETURN);
}

SymbolTableMap &SymbolTableMap::getInstance()
{
    static SymbolTableMap s_table;
    return s_table;
}

SymbolTableVec &SymbolTableVec::getInstance()
{
    static SymbolTableVec s_table;
    return s_table;
}

TokenType SymbolTableVec::LookupIdent(const std::string &key)
{
    SymbolTableVec &instance = getInstance();
    size_t size = distance(begin(instance.m_keywords), end(instance.m_keywords));
    for (size_t i = 0; i < size; ++i)
    {
        if (!key.compare(instance.m_keywords[i]))
        {
            return instance.m_symbols[i];
        }
    }

    return T_IDENT;
}