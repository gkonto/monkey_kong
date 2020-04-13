#include "token.hpp"

Token::Token(TokenType type, const char *ch)
     : type_(type), literal_(ch)
 {
 }

