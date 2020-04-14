#include <iostream>
#include <stdio.h>

#include "lexer.hpp"
#include "token.hpp"
//#include "symbols.hpp"


Lexer::Lexer(const std::string &input) :
    stream_(input)
{
}

bool Lexer::currentIsLetter()
{
    return (isalpha(ch_) || ch_ == '=') ? true : false;
}

// Takes upcoming char without extracting it 
// and updating ch_
char Lexer::nextChar()
{
    return stream_.peek();
}

// Gives next character and advances the position 
// in the input string
void Lexer::readChar()
{
    ch_ = stream_.get();

    if (stream_.eof() || stream_.tellg() == -1) {
        ch_ = 0;
    }
}

std::string Lexer::readNumber()
{
    std::string ret;
    while (isdigit(ch_)) {
        ret.append(1, ch_);
        readChar();
    }
    stream_.unget();

    return ret;
}

std::string Lexer::readIdentifier()
{
    std::string ret;
    while (currentIsLetter()) {
        ret.append(1, ch_);
        readChar();
    }

    stream_.unget();
    return ret;
}

void Lexer::skipWhitespace()
{
    while (ch_ == ' ' || ch_ == '\t' || ch_ == '\n' || ch_ == '\r') {
        readChar();
    }
}


//TODO refactoring
std::string Lexer::readString()
{
    std::string ret;
//    ret.append(1, ch_);
    readChar();

    while (ch_ != '"' && ch_ != 0) {
        ret.append(1, ch_);
        readChar();
    }

 //   ret.append(1, ch_);
    readChar();
    stream_.unget();

    return ret;
}

// Returned object must be freed explicitly
Token *Lexer::nextToken()
{
    Token *tok = nullptr;

    skipWhitespace();

    switch (ch_) {
        case '=':
            if (nextChar() == '=') {
                readChar();
                tok = new Token(T_EQ, "==");
            } else {
                tok = new Token(T_ASSIGN, ch_);
            }
            break;
        case ';':
            tok = new Token(T_SEMICOLON, ch_);
            break;
        case '-':
            tok = new Token(T_MINUS, ch_);
            break;
        case '!':
            if (nextChar() == '=') {
                readChar();
                tok = new Token(T_NOT_EQ, "!=");
            } else {
                tok = new Token(T_BANG, ch_);
            }
            break;
        case '[':
            tok = new Token(T_LBRACKET, ch_);
            break;
        case ']':
            tok = new Token(T_LBRACKET, ch_);
            break;
        case '/':
            tok = new Token(T_SLASH, ch_);
            break;
        case '*':
            tok = new Token(T_ASTERISK, ch_);
            break;
        case '<':
            tok = new Token(T_LT, ch_);
            break;
        case '>':
            tok = new Token(T_GT, ch_);
            break;
        case '(':
            tok = new Token(T_LPAREN, ch_);
            break;
        case ')':
            tok = new Token(T_RPAREN, ch_);
            break;
        case ',':
            tok = new Token(T_COMMA, ch_);
            break;
        case '+':
            tok = new Token(T_PLUS, ch_);
            break;
        case '{':
            tok = new Token(T_LBRACE, ch_);
            break;
        case '}':
            tok = new Token(T_RBRACE, ch_);
            break;
        case '"':
            tok = new Token(T_STRING, readString());
            break;
        case 0  :
            tok = new Token(T_EOF, "");
            break;
        default:
            if (currentIsLetter()) {
                std::string val(readIdentifier());
                //tok = new Token(SymbolTable::LookupIdent(val), val); //TODO enable
            } else if (isdigit(ch_)) {
                tok = new Token(T_INT, readNumber());
            } else {
                tok = new Token(T_ILLEGAL, 0);
            }
    }

    readChar();

    return tok;
}


