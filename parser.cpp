#include "parser.hpp"
#include "token.hpp"
#include "lexer.hpp"

Parser::Parser(Lexer *l) : lexer_(l)
{
    nextToken();
    nextToken();
}

Program *Parser::parseProgram()
{
    return nullptr;
}

void Parser::nextToken()
{
    cur_token_ = next_token_;
    next_token_ = lexer_->nextToken();  
}




