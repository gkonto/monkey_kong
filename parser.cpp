#include <memory>
#include "parser.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"

Parser::Parser(Lexer *l) : lexer_(l)
{
    nextToken();
    nextToken();
}


Node *Parser::parseStatement()
{
    switch (cur_token_->type()) {
        case T_LET:
            return parseLetStatement();
        default:
//            return parseExpressionStatement();
            break;
    }
    return nullptr;
}

bool Parser::peekTokenIs(TokenType type) const
{
    return next_token_->type() == type;
}


bool Parser::expectPeek(TokenType type)
{
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    } else {
        //peekError(type);
        return false;
    }
}

bool Parser::curTokenIs(TokenType type) const
{
    return cur_token_->type() == type;
}

Let *Parser::parseLetStatement()
{
    Let *l = new Let(cur_token_);

    if (!expectPeek(T_IDENT)) {
        return nullptr;
    }

     l->setName(new Identifier(cur_token_, cur_token_->literal()));

     if (!expectPeek(T_ASSIGN)) {
         return nullptr;
     }
     nextToken();

     //l->setValue(parseExpression(PL_LOWEST));

     if (peekTokenIs(T_SEMICOLON)) {
         nextToken();
     }

     return l;
}



std::unique_ptr<Program> Parser::parseProgram()
{
    std::unique_ptr<Program> prog = std::make_unique<Program>();

    while (cur_token_->type() != T_EOF) {
        Node *stmt = parseStatement();

        if (stmt) {
            prog->emplace_back(stmt);
        }
        nextToken();
    }

    return prog;
}

void Parser::nextToken()
{
    cur_token_ = next_token_;
    next_token_ = lexer_->nextToken();  
}




