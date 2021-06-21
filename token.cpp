#include <memory>
#include "token.hpp"

std::unique_ptr<Pool<Token>> TokenPool = nullptr;

void Token::dealloc(Token *tok)
{
    if (!tok)
        return;

#ifdef USE_POOL
    assert(TokenPool);
    if (TokenPool)
    {
        TokenPool->free(tok);
    }
    else
    {
        delete tok;
    }
#else
    delete tok;
#endif
}
