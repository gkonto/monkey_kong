#include <memory>
#include <iostream>
#include <unordered_map>
#include <functional>
#include "parser.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"

std::unordered_map<TokenType, PrecedenceLevel> Parser::Precedences;

Parser::Parser(Lexer *l) : lexer_(l)
{
    nextToken();
    nextToken();

    registerPrefix(T_IDENT, std::bind(&Parser::parseIdentifier, this));
    registerPrefix(T_INT, std::bind(&Parser::parseIntegerLiteral, this));
    registerPrefix(T_BANG, std::bind(&Parser::parsePrefixExpression, this));
    registerPrefix(T_MINUS, std::bind(&Parser::parsePrefixExpression, this));
    registerPrefix(T_FALSE, std::bind(&Parser::parseBoolean, this));
    registerPrefix(T_TRUE, std::bind(&Parser::parseBoolean, this));
    registerPrefix(T_LPAREN, std::bind(&Parser::parseGroupedExpression, this));
    registerPrefix(T_IF, std::bind(&Parser::parseIfExpression, this));
    registerPrefix(T_FUNCTION, std::bind(&Parser::parseFunctionLiteral, this));
    registerPrefix(T_STRING, std::bind(&Parser::parseStringLiteral, this));
    registerPrefix(T_LBRACKET, std::bind(&Parser::parseArrayLiteral, this));
    registerPrefix(T_LBRACE, std::bind(&Parser::parseHashLiteral, this));

    using std::placeholders::_1;
    registerInfix(T_PLUS, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_MINUS, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_SLASH, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_ASTERISK, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_EQ, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_NOT_EQ, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_LT, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_GT, std::bind(&Parser::parseInfixExpression, this, _1));
    registerInfix(T_LPAREN, std::bind(&Parser::parseCallExpression, this, _1));
    registerInfix(T_LBRACKET, std::bind(&Parser::parseIndexExpression, this, _1));

    initializePrecedence();
}

Node *Parser::parseIndexExpression(Node *left)
{
    IndexExpression *exp = new IndexExpression(cur_token_, left, nullptr);

    nextToken();
    exp->setIndex(parseExpression(PL_LOWEST));

    if (!expectPeek(T_RBRACKET))
    {
        return nullptr;
    }

    return exp;
}

Node *Parser::parseHashLiteral()
{
    HashLiteral *hash = new HashLiteral(cur_token_);

    while (!peekTokenIs(T_RBRACE))
    {
        nextToken();
        Node *key = parseExpression(PL_LOWEST);

        if (!expectPeek(T_COLON))
        {
            return nullptr;
        }

        nextToken();
        Node *value = parseExpression(PL_LOWEST);
        hash->emplace(key, value);

        if (!peekTokenIs(T_RBRACE) && !expectPeek(T_COMMA))
        {
            return nullptr;
        }
    }

    if (!expectPeek(T_RBRACE))
    {
        return nullptr;
    }

    return hash;
}

Node *Parser::parseArrayLiteral()
{
    std::vector<Node *> exp_list = parseExpressionList(T_RBRACKET);
    return new ArrayLiteral(cur_token_, exp_list);
}

Node *Parser::parseStringLiteral()
{
    return new StringLiteral(cur_token_);
}

std::vector<Identifier *> Parser::parseFunctionParameters()
{
    std::vector<Identifier *> identifiers;

    if (peekTokenIs(T_RPAREN))
    {
        nextToken();
        return identifiers;
    }

    nextToken();

    Identifier *ident = new Identifier(cur_token_, cur_token_->literal());
    identifiers.emplace_back(ident);

    while (peekTokenIs(T_COMMA))
    {
        nextToken();
        nextToken();
        Identifier *ident = new Identifier(cur_token_, cur_token_->literal());
        identifiers.emplace_back(ident);
    }

    if (!expectPeek(T_RPAREN))
    {
        std::cout << "FATAL parseFunctionParameters" << std::endl;
        return identifiers;
    }

    return identifiers;
}

Node *Parser::parseFunctionLiteral()
{
    FunctionLiteral *lit = new FunctionLiteral(cur_token_);

    if (!expectPeek(T_LPAREN))
    {
        return nullptr;
    }

    lit->setParameters(parseFunctionParameters());

    if (!expectPeek(T_LBRACE))
    {
        return nullptr;
    }

    lit->setBody(parseBlockStatement());

    return lit;
}

BlockStatement *Parser::parseBlockStatement()
{
    BlockStatement *block = new BlockStatement(cur_token_);
    nextToken();

    while (!curTokenIs(T_RBRACE) && !curTokenIs(T_EOF))
    {
        Node *stmt = parseStatement();
        if (stmt)
        {
            block->emplace_back(stmt);
        }
        nextToken();
    }
    return block;
}

Node *Parser::parseIfExpression()
{
    If *p_if = new If(cur_token_);

    if (!expectPeek(T_LPAREN))
    {
        return nullptr;
    }

    nextToken();
    p_if->setCondition(parseExpression(PL_LOWEST));

    if (!expectPeek(T_RPAREN))
    {
        return nullptr;
    }

    if (!expectPeek(T_LBRACE))
    {
        return nullptr;
    }

    p_if->setConsequence(parseBlockStatement());

    if (peekTokenIs(T_ELSE))
    {
        nextToken();

        if (!expectPeek(T_LBRACE))
        {
            return nullptr;
        }

        p_if->setAlternative(parseBlockStatement());
    }

    return p_if;
}

std::vector<Node *> Parser::parseExpressionList(TokenType end)
{
    std::vector<Node *> list;

    if (peekTokenIs(end))
    {
        nextToken();
        return list;
    }

    nextToken();
    list.emplace_back(parseExpression(PL_LOWEST));

    while (peekTokenIs(T_COMMA))
    {
        nextToken();
        nextToken();
        list.emplace_back(parseExpression(PL_LOWEST));
    }

    if (!expectPeek(end))
    {
        // FIXME This should return nullptr!!!
        return list;
    }

    return list;
}

Node *Parser::parseCallExpression(Node *function)
{
    CallExpression *exp = new CallExpression(cur_token_, function);
    exp->setArguments(parseExpressionList(T_RPAREN));
    return exp;
}

Node *Parser::parseGroupedExpression()
{
    nextToken();

    Node *exp = parseExpression(PL_LOWEST);

    if (!expectPeek(T_RPAREN))
    {
        return nullptr;
    }

    return exp;
}

Node *Parser::parseBoolean()
{
    return new Boolean(cur_token_, curTokenIs(T_TRUE));
}

Node *Parser::parseInfixExpression(Node *left)
{
    InfixExpression *inf_expression = new InfixExpression(cur_token_, left);
    PrecedenceLevel precedence = curPrecedence();
    nextToken();
    inf_expression->setRhs(parseExpression(precedence));

    return inf_expression;
}

Node *Parser::parsePrefixExpression()
{
    PrefixExpression *expression = new PrefixExpression(cur_token_);
    nextToken();

    expression->setRight(parseExpression(PL_PREFIX));
    return expression;
}

int Parser::parseInt(const std::string &input) const
{
    int value = 0;
    std::stringstream ss(input);
    ss >> value;

    return value;
}

Node *Parser::parseIntegerLiteral()
{
    int value = parseInt(cur_token_->literal());
    IntegerLiteral *p_lit = new IntegerLiteral(cur_token_, value);

    return p_lit;
}

Node *Parser::parseIdentifier()
{
    return new Identifier(cur_token_, cur_token_->literal());
}

void Parser::initializePrecedence()
{
    if (!Precedences.empty())
        return;

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

    if (entry == prefixParseFns_.end())
    {
        return nullptr;
    }

    return entry->second;
}

Parser::infixParseFn Parser::findInfix(TokenType type)
{
    auto entry = infixParseFns_.find(type);

    if (entry == infixParseFns_.end())
    {
        return nullptr;
    }

    return entry->second;
}

Node *Parser::parseExpression(PrecedenceLevel prec)
{
    prefixParseFn prefix_fun = findPrefix(cur_token_->type());

    if (!prefix_fun)
    {
        //noPrefixParseFnError(cur_token_->type_);
#ifdef DBG
        std::cout << "NO PREFIX EXPRESSION" << std::endl;
#endif
        return nullptr;
    }

    Node *leftExp = prefix_fun();

    while (!peekTokenIs(T_SEMICOLON) && prec < peekPrecedence())
    {
        infixParseFn infix_f = findInfix(next_token_->type());

        if (!infix_f)
        {
            return leftExp;
        }
        nextToken();
        leftExp = infix_f(leftExp);
    }

    return leftExp;
}

PrecedenceLevel Parser::peekPrecedence() const
{
    auto &prc = Parser::Precedences;
    auto entry = prc.find(next_token_->type());

    if (entry != prc.end())
    {
        return entry->second;
    }

    return PL_LOWEST;
}

PrecedenceLevel Parser::curPrecedence() const
{
    auto &prc = Parser::Precedences;
    auto entry = prc.find(cur_token_->type());

    if (entry != prc.end())
    {
        return entry->second;
    }

    return PL_LOWEST;
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

    statement->setReturnVal(parseExpression(PL_LOWEST));

    if (peekTokenIs(T_SEMICOLON))
    {
        nextToken();
    }

    return statement;
}

ExpressionStatement *Parser::parseExpressionStatement()
{
    ExpressionStatement *stmt = new ExpressionStatement();
    Node *exp = parseExpression(PL_LOWEST);
    stmt->setExpression(exp);

    if (peekTokenIs(T_SEMICOLON))
    {
        nextToken();
    }

    return stmt;
}

Node *Parser::parseStatement()
{
    switch (cur_token_->type())
    {
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
    if (peekTokenIs(type))
    {
        nextToken();
        return true;
    }
    else
    {
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

    if (!expectPeek(T_IDENT))
    {
        return nullptr;
    }

    l->setName(new Identifier(cur_token_, cur_token_->literal()));

    if (!expectPeek(T_ASSIGN))
    {
        return nullptr;
    }
    nextToken();

    l->setValue(parseExpression(PL_LOWEST));

    if (peekTokenIs(T_SEMICOLON))
    {
        nextToken();
    }

    return l;
}

std::unique_ptr<Program> Parser::parseProgram()
{
    std::unique_ptr<Program> prog = std::make_unique<Program>();

    while (cur_token_->type() != T_EOF)
    {
        Node *stmt = parseStatement();

        if (stmt)
        {
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
