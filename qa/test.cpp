#include <iostream>
#include <sstream>
#include <string>
#include <numeric>
#include <stdarg.h>
#include <memory>
#include "test.hpp"
#include "../token.hpp"
#include "../lexer.hpp"
#include "../ast.hpp"
#include "../parser.hpp"
#include "../pool.hpp"

using namespace std;

std::unique_ptr<Program> Test::parse(const std::string &input)
{
    Lexer lex(input);
    Parser parser(&lex);
    return std::unique_ptr<Program>(parser.parseProgram());
}


string Test::report_errors() const
{
    stringstream ss;
    for (auto &e : errors_) {
        ss << e;
    }
    ss << endl;

    return ss.str();
}


void Test::errorf(std::string input_case, const char *fmt, ...)
{
     char buffer[4096];
     va_list args;
     va_start(args, fmt);
     vsnprintf(buffer, sizeof(buffer), fmt, args);
     va_end(args);
     string msg;
     if (input_case.compare("")) {
         if (input_case.length() > 40) {
            msg.append("[!] Case -> " + input_case.substr(0, 40) + " ... \n");
         } else {
             msg.append("[!] Case -> " + input_case + "\n");
         }
     }
    msg.append(buffer);
    errors_.emplace_back(msg);
    is_failed_ = true;
}


void TestNextToken::execute()
{
    std::vector<Token> vec1 = {
        Token(T_ASSIGN, "="),
        Token(T_PLUS, "+"),
        Token(T_LPAREN, "("),
        Token(T_RPAREN, ")"),
        Token(T_LBRACE, "{"),
        Token(T_RBRACE, "}"),
        Token(T_COMMA, ","),
        Token(T_SEMICOLON, ";")
    };
    run_core("=+(){},;", vec1);

    std::string txt_test2("let five = 5;"\
                    "let ten = 10;"\
                    "let add = fn(x, y) {"\
                    "x + y;"\
                    "};"\
                    "let result = add(five, ten);"\
                    "!-/*5;"\
                    "5 < 10 > 5;"\
                    "if (5 < 10) {"\
                    "return true;"\
                    "} else {"\
                    "return false;"\
                    "}"\
                    "10 == 10;"\
                    "10 != 9;"\
                    "\"foobar\""\
                    "\"foo bar \""
                    "\" f oobar \""\
                    "[1, 2];"
                    "{\"foo\": \"bar\"}"
                    );

    std::vector<Token>vec2 = {
        Token(T_LET, "let"),      Token(T_IDENT, "five"),  Token(T_ASSIGN, "="),
        Token(T_INT, "5"),        Token(T_SEMICOLON, ";"), Token(T_LET, "let"),
        Token(T_IDENT, "ten"),    Token(T_ASSIGN, "="),    Token(T_INT, "10"),
        Token(T_SEMICOLON, ";"),  Token(T_LET, "let"),     Token(T_IDENT, "add"),
        Token(T_ASSIGN, "="),     Token(T_FUNCTION, "fn"), Token(T_LPAREN, "("),
        Token(T_IDENT, "x"),      Token(T_COMMA, ","),     Token(T_IDENT, "y"),
        Token(T_RPAREN, ")"),     Token(T_LBRACE, "{"),    Token(T_IDENT, "x"),
        Token(T_PLUS, "+"),       Token(T_IDENT, "y"),     Token(T_SEMICOLON, ";"),
        Token(T_RBRACE, "}"),     Token(T_SEMICOLON, ";"), Token(T_LET, "let"),
        Token(T_IDENT, "result"), Token(T_ASSIGN, "="),    Token(T_IDENT, "add"),
        Token(T_LPAREN, "("),     Token(T_IDENT, "five"),  Token(T_COMMA, ","),
        Token(T_IDENT, "ten"),    Token(T_RPAREN, ")"),    Token(T_SEMICOLON, ";"),
        Token(T_BANG, "!"),  Token(T_MINUS, "-")     , Token(T_SLASH, "/"),
        Token(T_ASTERISK, "*"),Token(T_INT, "5"),Token(T_SEMICOLON, ";"),
        Token(T_INT, "5"), Token(T_LT, "<"),Token(T_INT, "10"), Token(T_GT, ">"),
        Token(T_INT, "5"), Token(T_SEMICOLON, ";"), Token(T_IF, "if"),
        Token(T_LPAREN, "("), Token(T_INT, "5"), Token(T_LT, "<"), Token(T_INT, "10"),
        Token(T_RPAREN, ")"), Token(T_LBRACE, "{"), Token(T_RETURN, "return"), Token(T_TRUE, "true"),
        Token(T_SEMICOLON, ";"), Token(T_RBRACE, "}"), Token(T_ELSE, "else"), Token(T_LBRACE, "{"),
        Token(T_RETURN, "return"), Token(T_FALSE, "false"), Token(T_SEMICOLON, ";"), Token(T_RBRACE, "}"),
        Token(T_INT, "10"), Token(T_EQ, "=="), Token(T_INT, "10"), Token(T_SEMICOLON, ";"),
        Token(T_INT, "10"), Token(T_NOT_EQ, "!="), Token(T_INT, "9"), Token(T_SEMICOLON, ";"),
        Token(T_STRING, "foobar"), Token(T_STRING, "foo bar "),
        Token(T_STRING, " f oobar "), Token(T_LBRACKET, "["),
        Token(T_INT, "1"), Token(T_COMMA, ","),
        Token(T_INT, "2"), Token(T_RBRACKET, "]"),
        Token(T_SEMICOLON, ";"),
        Token(T_LBRACE, "{"), Token(T_STRING, "foo"),
        Token(T_COLON, ":"), Token(T_STRING, "bar"),
        Token(T_RBRACE, "}"),
        Token(T_EOF, "")
    };
    run_core(txt_test2, vec2);
}

static std::string appendLiterals(std::string &tok, const Token &tok1)
{
    return tok.append(tok1.literal());
}

void TestNextToken::run_core(std::string input, std::vector<Token> expec)
{
    Lexer lex(input);

    std::string expected = std::accumulate(
            expec.begin(),
            expec.end(),
            std::string(""),
            appendLiterals);

    for (auto &expected : expec) {
        Token *got = lex.nextToken();

        if (got && !(expected == *got)) {
            errorf(input, "[-] Expected: Type '%s' - Value: '%s'\n\t\tGot     : Type '%s' -  Value: '%s'",
                    tok_names[expected.type()], expected.literal().c_str(), tok_names[got->type()], got->literal().c_str());
            break;
        }
    }
}


TestLetStatements::TestLetStatements()
    : Test("TestLetStatements") {}


void TestLetStatements::execute()
{
    LetStatementCase<int> a("let x = 5;", "x", 5);
    run_core(a);
    LetStatementCase<bool> b("let y = true;", "y", true);
    run_core(b);
    LetStatementCase<std::string> c("let foobar = y;", "foobar", "y");
    run_core(c);
}


bool TestLetStatements::testLetStatement(const std::string &input, Node *statement, const std::string &name)
{
    if (!statement) {
        errorf(input, "parseProgram produced nullptr\n");
        return false;
    }

    if (statement->tokenLiteral().compare("let")) {
        errorf(input, "tokenLiteral not 'let'. got = '%s'\n", name.c_str(), statement->tokenLiteral().c_str());
        return false;
    }


    Let *p_let = dynamic_cast<Let *>(statement);
    if (!p_let) {
        errorf("", "not a Let statement!\n");
        return false;
    }

    if (name.compare(p_let->identName())) {
        errorf("", "Let::name_::value_ not %s. Got %s\n", 
                name.c_str(), p_let->identName().c_str());
        return false;
    }

    if (name.compare(p_let->name()->tokenLiteral())) {
        errorf("", "name not '%s'. got '%s'\n", p_let->name()->tokenLiteral().c_str(), name.c_str());
        return false;
    }

    return true;
}


bool Test::testIdentifier(const std::string &input, Node *exp, const std::string &value)
{
    Identifier *ident = dynamic_cast<Identifier *>(exp);
    if (!ident) {
        errorf(input, "exp not Identifier");
        return false;
    }

    if (value.compare(ident->value())) {
        errorf(input, "exp->value_ not %s. got %s", value.c_str(), ident->value().c_str());
        return false;
    }
    
    if (value.compare(ident->tokenLiteral())) {
        errorf(input, "ident->tokenLiteral not %s. got %s", value.c_str(), ident->tokenLiteral().c_str());
        return false;
    }

    return true;
}

bool Test::testBooleanLiteral(const std::string &input, Node *il, bool value)
{
    errorf(input, "missing testIntegerLiteral\n");
    return false;
    /*)
    Boolean *integ = dynamic_cast<Boolean *>(il);

    if (!il) {
        errorf(input, "il not Boolean. got = %s", integ->asString().c_str());
        return false;
    }

    if (integ->value() != value) {
        errorf(input, "value not %d. got = %d\n", value, integ->value());
        return false;
    }

    if (integ->tokenLiteral() != integ->asString()) {
        errorf(input, "TokenLiteral not %s. got = %s", value, integ->tokenLiteral().c_str());
        return false;
    }
    */

    return true;
}


bool Test::testIntegerLiteral(const std::string &input, Node *il, int value) 
{
    errorf(input, "missing testIntegerLiteral\n");
    return false;
    /*
    IntegerLiteral *integ = dynamic_cast<IntegerLiteral *>(il);
    if (!il) {
        errorf(input, "il not IntegerLiteral. got = %s", integ->asString().c_str());
        return false;
    }

    if (integ->value() != value) {
        errorf(input, "value not %d. got = %d\n", value, integ->value());
        return false;
    }

    if (integ->tokenLiteral() != integ->asString()) {
        errorf(input, "TokenLiteral not %s. got = %s", value, integ->tokenLiteral().c_str());
        return false;
    }
    */

    return true;
}


bool Test::testLiteralExpression(const std::string &input, Node *exp, bool expected)
{
    return testBooleanLiteral(input, exp, expected);
}

bool Test::testLiteralExpression(const std::string &input, Node *exp, int expected)
{
    return testIntegerLiteral(input, exp, expected);
}

bool Test::testLiteralExpression(const std::string &input, Node *exp, std::string &expected)
{
    return testIdentifier(input, exp, expected);
}

template<typename T>
void TestLetStatements::run_core(LetStatementCase<T> c)
{
    std::unique_ptr<Program> program(parse(c.input_));

    if (!program) {
        errorf(c.input_, "parseProgram returned nullptr\n");
        return;
    }

    if (program->size() != 1) {
        errorf(c.input_, "program.Statements does not contain 1 statements. got %d\n", program->size());
        return;
    }

    Node *stmt = (*program)[0];
    if (!testLetStatement(c.input_, stmt, c.expectedIdentifier_)) {
        return;
    }

    /*
     TODO
    Let *l_s = dynamic_cast<Let *>(stmt);
    Node *val = l_s->value();
    
    if (!testLiteralExpression(c.input_, val, c.expectedValue_)) {
        return;
    }
    */
}


void TestPoolArena::execute()
{
    Token *tok = Token::alloc(T_EOF, "1");
    Token *tok1 = Token::alloc(T_EOF, "2");
    Token::alloc(T_EOF, "3");
    Token::alloc(T_EOF, "4");
    Token::alloc(T_EOF, "5");
    Token::alloc(T_EOF, "6");
    Token::alloc(T_EOF, "7");
    Token::alloc(T_EOF, "8");
    Token::alloc(T_EOF, "9");
    Token::alloc(T_EOF, "10");
    Token::alloc(T_EOF, "11");
    Token::alloc(T_EOF, "12");
    Token::alloc(T_EOF, "13");
    Token::alloc(T_EOF, "14");
    Token::alloc(T_EOF, "15");
    Token::alloc(T_EOF, "16");
    Token::alloc(T_EOF, "17");
    Token::alloc(T_EOF, "18");
    Token::alloc(T_EOF, "18");
    Token::alloc(T_EOF, "20");

    Token::dealloc(tok);
    Token::dealloc(tok1);
    Token::alloc(T_EOF, "21");
    Token::alloc(T_EOF, "22");


    TokenPool.reset();
}


