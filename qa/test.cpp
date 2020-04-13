#include <iostream>
#include <sstream>
#include <string>
#include <numeric>
#include <stdarg.h>
#include "test.hpp"
#include "token.hpp"

using namespace std;

map<string, Test *> tests;

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
     string msg(buffer);
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

void TestNextToken::run_core(std::string input, std::vector<Token> expec)
{
    errorf(input, "Missing function %s", "lala");
    /*
    Lexer lex(input);

    std::string expected = std::accumulate(
            expec.begin(),
            expec.end(),
            std::string(""),
            appendLiterals);

    for (auto &expected : expec) {
        Token *got = lex.nextToken();

        if (got && !(expected == *got)) {
            ErrorHandler::setErrorMsg("[-] Expected: Type '%s' - Value: '%s'\n\t\tGot     : Type '%s' -  Value: '%s'",
                    tokens_name[expected.type_], expected.literal_.c_str(), tokens_name[got->type_], got->literal_.c_str());
            break;
        }
    }
    */
}


