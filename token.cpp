#include <memory>
#include "token.hpp"

std::unique_ptr<Pool<Token>> TokenPool = nullptr;

int num_allocs = 0;

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

void Token::dealloc(Token *tok)
{
    if (!tok) return;

#ifdef USE_POOL
    assert(TokenPool);
    if (TokenPool) {
        TokenPool->free(tok);
    } else {
        delete tok;
    }
#else
    delete tok;
#endif
}


