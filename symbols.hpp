#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include <map>
#include <string>

#include "token.hpp"

#define SYMBOL_TOKENS                             \
    X(ST_FUNCTION, std::string("fn"), T_FUNCTION) \
    X(ST_LET, std::string("let"), T_LET)          \
    X(ST_TRUE, std::string("true"), T_TRUE)       \
    X(ST_FALSE, std::string("false"), T_FALSE)    \
    X(ST_IF, std::string("if"), T_IF)             \
    X(ST_ELSE, std::string("else"), T_ELSE)       \
    X(ST_RETURN, std::string("return"), T_RETURN)

#define X(a, b, c) a,
enum SymbolTokens
{
    SYMBOL_TOKENS
        ST_MAX
};
#undef X

class SymbolTableMap
{
public:
    static SymbolTableMap &getInstance();
    static TokenType LookupIdent(const std::string &key);

private:
    SymbolTableMap() { initializeSymbols(); }
    void initializeSymbols();
    std::map<std::string, TokenType> keywords;
};

class SymbolTableVec
{
public:
    static SymbolTableVec &getInstance();
    static TokenType LookupIdent(const std::string &key);

private:
#define X(a, b, c) b,
    const std::string m_keywords[ST_MAX] = {SYMBOL_TOKENS};
#undef X

#define X(a, b, c) c,
    TokenType m_symbols[ST_MAX] = {SYMBOL_TOKENS};
#undef X
};


TokenType LookupIdentifier(const std::string &key);


#endif
