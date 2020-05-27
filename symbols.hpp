#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include <map>
#include <string>

#include "token.hpp"

class SymbolTable
{
    public:
        static SymbolTable &getInstance();
        static TokenType LookupIdent(const std::string &key);

    private:
        SymbolTable() { initializeSymbols(); }
        void initializeSymbols();
        std::map<std::string, TokenType> keywords;
};


#endif
