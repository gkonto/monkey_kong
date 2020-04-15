#include <memory>
#include <iostream>
#include <unordered_map>
#include "parser.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"

std::unordered_map<TokenType, PrecedenceLevel> Parser::Precedences;


Parser::Parser(Lexer *l) : lexer_(l)
{
    nextToken();
    nextToken();
    initializePrecedence();
}

 void Parser::initializePrecedence()
 {
     if (!Precedences.empty()) return;
 
     Precedences.emplace(T_EQ, PL_EQUALS);
     Precedences.emplace(T_NOT_EQ, PL_EQUALS);
     Precedences.emplace(T_LT, PL_LESSGREATER);
     Precedences.emplace(T_GT, PL_LESSGREATER);
     Precedences.emplace(T_PLUS, PL_SUM);
     Precedences.emplace(T_MINUS, PL_SUM);
     Precedences.emplace(T_SLASH, PL_PRODUCT);
     Precedences.emplace(T_ASTERISK, PL_PRODUCT);
     Precedences.emplace(T_LPAREN, PL_CALL);
     Precedences.emplace(T_LBRACKET, PL_INDEX);
 }


 Parser::prefixParseFn Parser::findPrefix(TokenType type)
 {
     auto entry = prefixParseFns_.find(type);

     if (entry == prefixParseFns_.end()) {
         return nullptr;
     }

     return entry->second;
 }

 Parser::infixParseFn Parser::findInfix(TokenType type)
 {
     auto entry = infixParseFns_.find(type);
 
     if (entry == infixParseFns_.end()) {
         return nullptr;
     }
         
     return entry->second;
 } 

Node *Parser::parseExpression(PrecedenceLevel prec)
 {
     prefixParseFn prefix_fun = findPrefix(cur_token_->type());

     if (!prefix_fun) {
         //noPrefixParseFnError(cur_token_->type_);
         return nullptr;
     }

      Node *leftExp = prefix_fun();

      /*
     while (!peekTokenIs(T_SEMICOLON) && prec < peekPrecedence()) {
         infixParseFn infix_f = findInfix(next_token_->type_);

         if (!infix_f) {
             return leftExp;
         }
         nextToken();
         leftExp = infix_f(leftExp);
     }
     */

     return leftExp;
 }


 void Parser::registerPrefix(TokenType type, prefixParseFn fun)
 {
     prefixParseFns_.emplace(type, fun);
 }


 void Parser::registerInfix(TokenType type, infixParseFn fun)
 {
     infixParseFns_.emplace(type, fun);
 }


 Return *Parser::parseReturnStatement()
 {
     Return *statement = new Return(cur_token_, nullptr);
     nextToken();

     //statement->setReturnVal(parseExpression(PL_LOWEST));

     if (peekTokenIs(T_SEMICOLON)) {
         nextToken();
     }

     return statement;
 }

 ExpressionStatement *Parser::parseExpressionStatement()
 {
     ExpressionStatement *stmt = new ExpressionStatement();
     Node *exp = parseExpression(PL_LOWEST);
     stmt->setExpression(exp);

     if (peekTokenIs(T_SEMICOLON)) {
         nextToken();
     }

     return stmt;
 }


Node *Parser::parseStatement()
{
    switch (cur_token_->type()) {
        case T_LET:
            return parseLetStatement();
        case T_RETURN:
             return parseReturnStatement();
        default:
            return parseExpressionStatement();
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




