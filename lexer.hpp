#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <sstream>

struct Token;

// FIXME Does not support doubles!
class Lexer
{
    public:
        explicit Lexer(const std::string &input);
        Token *nextToken();
        int position() { return stream_.tellg(); }
        int readPosition() { return stream_.tellg(); }
        char current() { return ch_; }
    private:
        void readChar();
        char nextChar();
        std::string readIdentifier();
        std::string readNumber();
        bool currentIsLetter();

        void skipWhitespace();

        std::string readString();

        std::stringstream stream_;
        int ch_ = ' ';           // Currect char under examination
};


#endif
