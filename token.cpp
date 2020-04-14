#include "token.hpp"

Token::Token(TokenType type, const char *ch)
     : type_(type), literal_(std::string(ch))
 {
 }

Token::Token(TokenType type, int ch)
     : type_(type), literal_(std::string(1, ch))
 {
 }

bool Token::operator==(const Token &b) const
{
    if (type_ == b.type_ && !literal_.compare(b.literal_)) {
        return true;
    }

    return false;
}

Token::Token(TokenType type, std::string lit)
     : type_(type), literal_(lit)
 {
 }


