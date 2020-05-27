#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include <numeric>
#include <string.h>
#include <stdarg.h>
#include <memory>
#include "test.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "env.hpp"
#include "pool.hpp"
#include "object.hpp"

using namespace std;
using namespace std::chrono;

std::unique_ptr<Program> Test::parse(const std::string &input)
{
    Lexer lex(input);
    Parser parser(&lex);
    return parser.parseProgram();
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


void Test::msgf(std::string input_case, const char *fmt, ...)
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

    return true;
}


bool Test::testIntegerLiteral(const std::string &input, Node *il, int value) 
{
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

bool Test::testLiteralExpression(const std::string &input, Node *exp, const char *expected)
{
    std::string v(expected);
    return testIdentifier(input, exp, v);
}

template<typename T>
void TestLetStatements::run_core(LetStatementCase<T> c)
{
    std::unique_ptr<Program> program = parse(c.input_);

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

    Let *l_s = dynamic_cast<Let *>(stmt);
    Node *val = l_s->value();
    
    if (!testLiteralExpression(c.input_, val, c.expectedValue_)) {
        return;
    }
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


    //TokenPool.reset();
}


 void TestReturnStatements::execute()
 {
     run_core("return 10;", 10);
     run_core("return 10; 9;", 10);
     run_core("return 2 * 5; 9;", 10);
     run_core("9; return 2 * 5; 9;", 10);
     run_core("if (10 > 1) {"
                 "if (10 > 1) {"
                     "return 10;"
                 "}"
                 "return 1"
             "}", 10);
 }


void TestReturnStatements::run_core(std::string input, int expected)
 {
     std::unique_ptr<Program> program = parse(input);
     if (!program) {
         errorf(input, "Program is nullptr\n");
         return;
     }

     if (program->size() == 0) {
         errorf(input, "Program returned empty!\n");
         return;
     }

     for (auto &stmt : *program) {
        Return *ret = dynamic_cast<Return *>(stmt);        
        if (!ret) {
            continue;
        }

        if (ret->tokenLiteral() != "return") {
            errorf(input, "Return * tokenLiteral not 'return'. got %s\n", ret->tokenLiteral());
        }
     }
 }


void TestIntegerLiteralExpression::execute()
 {
     std::string input("5;");
     std::unique_ptr<Program> program = parse(input);

     if (program->size() != 1) {
         errorf(input, "program has not enough statements. got %d\n", program->size());
     }

     ExpressionStatement *p_exp_state = dynamic_cast<ExpressionStatement *>((*program)[0]);
     if (!p_exp_state) {
         errorf(input, "exp not ExpressionStatement!\n");
         return;
     }

     IntegerLiteral *p_literal = dynamic_cast<IntegerLiteral *>(p_exp_state->expression());
     if (!p_literal) {
         errorf(input, "exp not IntegerLiteral!\n");
         return;
     }

     if (p_literal->value() != 5) {
         errorf(input, "literal.Value not %d. got = %d\n", 5, p_literal->value());
         return;
     }

     if (p_literal->tokenLiteral() != "5") {
         errorf(input, "literal.TokenLiteral not %s. got %s\n", "5", p_literal->tokenLiteral().c_str());
         return;
     }

 }





 void TestIdentifierExpression::execute()
 {
     std::string input = "foobar;";
     std::unique_ptr<Program> program = parse(input);
 
     if (program->size() != 1) {
         errorf(input, "program should have 1 statement. got %d\n", program->size());
         return;
     }
 
     ExpressionStatement *es = dynamic_cast<ExpressionStatement *>((*program)[0]);
     if (!es) {
         errorf(input, "program[0] is not ExpressionStatement!\n");
         return;
     }
 
     Identifier *ident = dynamic_cast<Identifier *>(es->expression());
     if (!ident) {
         errorf(input, "not identifier!\n");
         return;
     }
 
     if (ident->value() != "foobar") {
         errorf(input, "ident.Value not foobar. got %s\n", ident->value().c_str());
         return;
     }
 
     if (ident->tokenLiteral() != "foobar") {
         errorf(input, "ident->tokenLiteral not foobar. got %s\n", ident->tokenLiteral().c_str());
         return;
     }
 }
 


 void TestParsingPrefixExpression::execute()
 {
     run_core(PrefixTest<int>("!5;", "!", 5));
     run_core(PrefixTest<int>("-15;", "-", 15));
     run_core(PrefixTest<bool>("!true;", "!", true));
     run_core(PrefixTest<bool>("!false;", "!", false));
 }
 
 template<typename T>
 void TestParsingPrefixExpression::run_core(PrefixTest<T> a)
 {
     std::unique_ptr<Program> program = parse(a.input_);
 
     if (program->size() != 1) {
        errorf(a.input_, "program does not contain %d statements. got %d\n", 1, program->size());
        return;
     }
 
     ExpressionStatement *p_stmt = dynamic_cast<ExpressionStatement *>((*program)[0]);
     if (!p_stmt) {
        errorf(a.input_, "program[0] not ExpressionStatement");
        return;
     }
 
     PrefixExpression *p_prExp = dynamic_cast<PrefixExpression *>(p_stmt->expression());
     if (!p_prExp) {
        errorf(a.input_, "smtm not PrefixExpression");
        return;
     }
 
     if (p_prExp->operator_s().compare(a.operator_)) {
        errorf(a.input_, "Operator is not %s. got %s\n",
                a.operator_.c_str(), p_prExp->operator_s().c_str());
        return;
     }
 
     if (!testLiteralExpression(a.input_, p_prExp->right(), a.value_)) {
        return;
     }
 }


 void TestParsingInfixExpressions::execute()
 {
     run_core(InfixTest<int, int>("5 + 5;", 5, "+", 5));
     run_core(InfixTest<int, int>("5 - 5", 5, "-", 5));
     run_core(InfixTest<int, int>("5 * 5", 5, "*", 5));
     run_core(InfixTest<int, int>("5 / 5", 5, "/", 5));
     run_core(InfixTest<int, int>("5 > 5", 5, ">", 5));
     run_core(InfixTest<int, int>("5 < 5", 5, "<", 5));
     run_core(InfixTest<int, int>("5 == 5", 5, "==",  5));
     run_core(InfixTest<int, int>("5 != 5", 5, "!=", 5));
     run_core(InfixTest<bool, bool>("true == true", true, "==", true));
     run_core(InfixTest<bool, bool>("true != false", true, "!=", false));
     run_core(InfixTest<bool, bool>("false == false", false, "==", false));
 }

 template<typename T, typename C>
 void TestParsingInfixExpressions::run_core(InfixTest<T, C> a)
 {
     std::unique_ptr<Program>  program = parse(a.input_);

     if (program->size() != 1) {
         errorf(a.input_, "program does not contain %d statements. got %d\n", 1, program->size());
     }

     ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>((*program)[0]);
     if (!stmt) {
        errorf(a.input_, "program[0] not ExpressionStatement\n");
        return;
     }

     InfixExpression *exp = dynamic_cast<InfixExpression *>(stmt->expression());

     if (!exp) {
         errorf(a.input_, "stmt not InfixExpression\n");
         return;
     }

     if (a.op_.compare(tok_names[exp->op()])) {
         errorf(a.input_, "Operator is not %s. got = %s\n", a.op_.c_str(), tok_names[exp->op()]);
         return;
     }

     if (!testLiteralExpression(a.input_, exp->lhs(), a.lhs_)) {
         return;
     }

     if (!testLiteralExpression(a.input_, exp->rhs(), a.rhs_)) {
        return;
    }
 }


 TestOperatorPrecedenceParsing::TestOperatorPrecedenceParsing()
     : Test("TestOperatorPrecedenceParsing")
 {
     using OP = OperatorPrecedence;
     tests_.emplace_back(OP("-a * b", "((-a) * b)"));
     tests_.emplace_back(OP("!-a", "(!(-a))"));
     tests_.emplace_back(OP("a + b + c", "((a + b) + c)"));
     tests_.emplace_back(OP("a + b - c", "((a + b) - c)"));
     tests_.emplace_back(OP("a * b * c", "((a * b) * c)"));
     tests_.emplace_back(OP("a * b / c", "((a * b) / c)"));
     tests_.emplace_back(OP("a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"));
     tests_.emplace_back(OP("3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"));
     tests_.emplace_back(OP("5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"));
     tests_.emplace_back(OP("5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"));
     tests_.emplace_back(OP("3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"));
     tests_.emplace_back(OP("3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"));
     tests_.emplace_back(OP("true", "true"));
     tests_.emplace_back(OP("false", "false"));
     tests_.emplace_back(OP("3 > 5 == false", "((3 > 5) == false)"));
     tests_.emplace_back(OP("3 < 5 == true", "((3 < 5) == true)"));
     tests_.emplace_back(OP( "1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"));
     tests_.emplace_back(OP("(5 + 5) * 2", "((5 + 5) * 2)" ));
     tests_.emplace_back(OP("2 / (5 + 5)", "(2 / (5 + 5))"));
     tests_.emplace_back(OP("-(5 + 5)", "(-(5 + 5))"));
     tests_.emplace_back(OP("!(true == true)", "(!(true == true))"));
     tests_.emplace_back(OP("a + add(b * c) + d", "((a + add((b * c))) + d)"));
     tests_.emplace_back(OP("add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"));
     tests_.emplace_back(OP("add(a + b + c * d / f + g)","add((((a + b) + ((c * d) / f)) + g))"));
     tests_.emplace_back(OP("a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"));
     tests_.emplace_back(OP("add(a * b[2], b[1], 2 * [1, 2][1])", "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"));
 }

 void TestOperatorPrecedenceParsing::execute()
 {
     for (auto &a : tests_) {
         std::unique_ptr<Program>  program = parse(a.input_);
         std::string actual(program->asString());

         if (actual.compare(a.expected_)) {
             errorf("expected: %s, got: %s\n", a.expected_.c_str(), actual.c_str());
         }
     }
 }




 void TestIfExpression::execute()
 {
     std::string input("if (x < y) { x }");
     std::unique_ptr<Program> program = parse(input);

     if (program->size() != 1) {
         errorf(input, "program does not contain %d statements. got %d\n", 1, program->size());
     }

     ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>((*program)[0]);
     if (!stmt) {
        errorf(input, "program[0] not ExpressionStatement\n");
        return;
     }

     If *exp = dynamic_cast<If *>(stmt->expression());

     if (!exp) {
         errorf(input, "stmt not If expression\n");
         return;
     }

     if (!testInfixExpression<std::string>(input, exp->condition(), "x", "<", "y")) {
         return;
     }

     if (!exp->consequence()) {
         errorf(input, "Consequence cannot be null\n");
         return;
     }

     if (exp->consequence()->size() != 1) {
         errorf(input, "Consequence is not 1 statements. got = %d\n", exp->consequence()->size());
         return ;
     }

     BlockStatement * bl_statement = exp->consequence();
     ExpressionStatement *consequence = dynamic_cast<ExpressionStatement *>((*bl_statement)[0]);

     if (!testIdentifier(input, consequence->expression(), "x")) {
         return;
     }

     if (exp->alternative() != nullptr) {
         errorf(input, "exp.AlternativeStatements was not nullptr. got = %s\n", exp->alternative()->asString().c_str());
         return;
     }

 }

 void TestFunctionLiteralParsing::execute()
 {
     std::string input("fn(x, y) { x + y;}");
 
     std::unique_ptr<Program>  program = parse(input);
 
     if (program->size() != 1) {
         errorf(input, "program does not contain %d statements. got %d\n", 1, program->size());
     }
 
     ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>((*program)[0]);
     if (!stmt) {
        errorf(input, "program[0] not ExpressionStatement\n");
        return;
     }
 
     FunctionLiteral *function = dynamic_cast<FunctionLiteral *>(stmt->expression());
     if (!function) {
         errorf(input, "Expression is not FunctionLiteral\n");
         return;
     }
 
     if (function->paramSize() != 2) {
         errorf(input, "Function Literal wrong. want 2 parameters. got %d\n", function->paramSize());
         return;
     }
 
     testLiteralExpression(input, function->param(0), "x");
     testLiteralExpression(input, function->param(1), "y");
 
     BlockStatement * body = function->body();
     if (body->size() != 1) {
         errorf(input, "function.Body.Statements has not 1 statements. got %d\n", body->size());
         return;
     }
 
     ExpressionStatement *bodyStmt = dynamic_cast<ExpressionStatement *>((*body)[0]);
     if (!bodyStmt) {
         errorf(input, "function body stmt is not ExpressionStatement. got = %d\n");
         return;
     }
 
     testInfixExpression(input, bodyStmt->expression(), "x", "+", "y");
 }
 
 void TestFunctionParametersParsing::execute()
 {
     std::vector<std::string> case1;
     run_case("fn() {};", case1);
 
     std::vector<std::string> case2;
     case2.emplace_back("x");
     run_case("fn(x) {};", case2);
 
     std::vector<std::string> case3;
     case3.emplace_back("x");
     case3.emplace_back("y");
     case3.emplace_back("z");
     run_case("fn(x, y, z) {};", case3);
 }
 
 
 void TestFunctionParametersParsing::run_case(const std::string input, const std::vector<std::string> &expectedParams)
 {
     std::unique_ptr<Program>  program = parse(input);
 
     ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>((*program)[0]);
     FunctionLiteral *function = dynamic_cast<FunctionLiteral *>(stmt->expression());
  
     if (function->paramSize() != expectedParams.size()) {
         errorf(input, "length parameters wrong. want %d, got %d\n", expectedParams.size(), function->paramSize());
         return;
     }
 
     int i = 0;
     for (auto a : expectedParams) {
         testLiteralExpression(input, function->param(i++), a);
     }
 }


 void TestCallExpressionParsing::execute()
 {
     std::string input("add(1, 2 * 3, 4 + 5);");

     std::unique_ptr<Program>  program = parse(input);

     if (program->size() != 1) {
         errorf(input, "program.Statements does not contain %d statements. got %d\n", 1, program->size());
         return;
     }

     ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>((*program)[0]);
     if (!stmt) {
         errorf(input, "stmt is not ExpressionStatement\n");
         return;
     }

     CallExpression *exp = dynamic_cast<CallExpression *>(stmt->expression());
     if (!exp) {
         errorf(input, "stmt.Expression is not CallExpression\n");
         return;
     }

     if (!testIdentifier(input, exp->function(), "add")) {
         return;
     }

     if (exp->size() != 3) {
         errorf(input, "wrong length of arguments. got %d\n", exp->size());
         return;
     }

     testLiteralExpression(input, exp->argNum(0), 1);
     testInfixExpression(input, exp->argNum(1), 2, "*", 3);
     testInfixExpression(input, exp->argNum(2), 4, "+", 5);
 }


 bool Test::testIntegerObject(const std::string &input, Single *obj, int expected)
 {
     if (!obj) {
         errorf(input, "Evaluated obj is nullptr\n");
         return false;
     }

     //Integer *result = dynamic_cast<Integer *>(obj);


     if (obj->type_ != INTEGER) {
         errorf(input, "object is not Integer.got %s\n", object_name[obj->type_]);
         return false;
     }

     if (obj->data.i.value_ != expected) {
         errorf(input, "object value was wrong %d. got %d\n", expected, obj->data.i.value_);
         return false;
     }

     return true;
 }


Single *Test::eval(const std::string &input, Environment &env)
{
     Lexer l(input);
     Parser p(&l);
     std::unique_ptr<Program> program = p.parseProgram();

#ifdef USEVISITOR
     Evaluator evaluator(program.get(), &env);
     Single *ret = evaluator.eval();
#else
     Single *ret = program->eval(&env);
#endif
     return ret;
}

void TestEvalIntegerExpression::run_core(std::string input, int expected)
 {
     Environment env;
     Single *evaluated = eval(input, env);
     testIntegerObject(input, evaluated, expected);
     evaluated->release();
 }

 void TestEvalIntegerExpression::execute()
 {
     run_core("10", 10);
     run_core("5", 5);
     run_core("-5", -5);
     run_core("-10", -10);
     run_core("5 + 5 + 5 + 5 - 10", 10);
     run_core("2 * 2 * 2 * 2 * 2", 32);
     run_core("-50 + 100 + -50", 0);
     run_core("5 * 2 + 10", 20);
     run_core("5 + 2 * 10", 25);
     run_core("20 + 2 * -10", 0);
     run_core("50 / 2 * 2 + 10", 60);
     run_core("2 * (5 + 10)", 30);
     run_core("3 * 3 * 3 + 10", 37);
     run_core("3 * (3 * 3) + 10", 37);
     run_core("(5 + 10 * 2 + 15 / 3) * 2 + -10", 50);
     run_core("let a = 5; a;", 5);
     run_core("let a = 5 * 5; a;", 25);
     run_core("let a = 5; let b = a; b;", 5);
     run_core("let a = 5; let b = a; let c = a + b + 5; c;", 15);
     run_core("let a = 5; let b = a; let c = a + b + 5; a;", 5);
     run_core("let a = 5; let b = a; let c = a + b + 5; a;", 5);
 }

void TestEvalBooleanExpression::execute()
{
    run_core("true", true);
    run_core("false", false);
    run_core("1 < 2", true);
    run_core("1 > 2", false);
    run_core("1 < 1", false);
    run_core("1 > 1", false);
    run_core("1 == 1", true);
    run_core("1 != 1", false);
    run_core("1 == 2", false);
    run_core("1 != 2", true);
    run_core("true == true", true);
    run_core("false == false", true);
    run_core("true == false", false);
    run_core("true != false", true);
    run_core("false != true", true);
    run_core("(1 < 2) == true", true);
    run_core("(1 < 2) == false", false);
    run_core("(1 > 2) == true", false);
    run_core("(1 > 2) == false", true);
}


void TestEvalBooleanExpression::run_core(std::string input, bool expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testBooleanObject(input, evaluated, expected);
    evaluated->release();
}


bool Test::testBooleanObject(const std::string &input, Single *obj, bool expected)
{
    if (!obj) {
        errorf(input, "Evaluated obj is nullptr\n");
        return false;
    }

    //Bool *result = dynamic_cast<Bool *>(obj);

    if (obj->type_ != BOOLEAN) {
        errorf(input, "object is not Boolean.got %s\n", object_name[obj->type_]);
        return false;
    }

    if (obj->data.b.value_ != expected) {
        errorf(input, "object has wrong value. got=%d, want=%d\n", input.c_str(), obj->data.b.value_, expected);
        return false;
    }
    return true;
}


void TestBangOperator::execute()
{
    run_core("!true", false);
    run_core("!false", true);
    run_core("!5", false);
    run_core("!!true", true);
    run_core("!!false", false);
    run_core("!!5", true);
}

void TestBangOperator::run_core(std::string input, bool expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testBooleanObject(input, evaluated, expected);
    evaluated->release();
}



void TestIfElseExpressions::execute()
{
    run_core("if (true) { 10 }", 10);
    run_core("if (false) { 10 }");
    run_core("if (1) { 10 }", 10);
    run_core("if (1 < 2) { 10 }", 10);
    run_core("if (1 > 2) { 10 }");
    run_core("if (1 > 2) { 10 } else { 20 }", 20);
    run_core("if (1 < 2) { 10 } else { 20 }", 10);
}


bool Test::testNullObject(const std::string &input, Single *obj)
{
    if (obj != &Model::null_o) {
        errorf(input, "object is not NULL.");
        return false;
    }

    return true;
}

void TestIfElseExpressions::run_core(std::string input)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testNullObject(input, evaluated);
}

void TestIfElseExpressions::run_core(std::string input, int expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testIntegerObject(input, evaluated, expected);
    evaluated->release();
}



void TestEvalReturnStatements::execute()
{
    run_core("return 10;", 10);
    run_core("return 10; 9;", 10);
    run_core("return 2 * 5; 9;", 10);
    run_core("9; return 2 * 5; 9;", 10);
    run_core("if (10 > 1) {"
                "if (10 > 1) {"
                    "return 10;"
                "}"
                "return 1"
            "}", 10);
}


void TestEvalReturnStatements::run_core(std::string input, int expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testIntegerObject(input, evaluated, expected);
    evaluated->release();
}


void TestErrorHandler::execute()
{
    run_core("5 + true;", "type mismatch: INTEGER + BOOLEAN");
    run_core("5 + true;", "type mismatch: INTEGER + BOOLEAN");
    run_core("5 + true;", "type mismatch: INTEGER + BOOLEAN");
    run_core( "5 + true; 5;","type mismatch: INTEGER + BOOLEAN");
    run_core("-true","unknown operator: -BOOLEAN");
    run_core("true + false;","unknown operator: BOOLEAN + BOOLEAN");
    run_core("5; true + false; 5","unknown operator: BOOLEAN + BOOLEAN");
    run_core("if (10 > 1) { true + false; }","unknown operator: BOOLEAN + BOOLEAN");
    run_core("if (10 > 1) {"
                "if (10 > 1) {"
                    "return true + false;"
                "}"
                "return 1;"
            "}","unknown operator: BOOLEAN + BOOLEAN");
    run_core("foobar", "identifier not found: foobar");
    run_core("\"Hello\" - \"World\"", "unknown operator: STRING - STRING");
    run_core("{\"name\": \"Monkey\"}[fn(x) { x }];", "unusable as hash key: FUNCTION");
}


void TestErrorHandler::run_core(std::string input, std::string expected_e)
{
    Environment env;
    Single *evaluated = eval(input, env);
    if (!evaluated) {
        errorf(input, "evaluted object is nullptr\n");
        return;
    }

    if (evaluated->type_ != ERROR) {
        errorf(input, "no error object returned.\n");
        return;
    }

    if (expected_e.compare(evaluated->data.e.msg_)) {
        errorf(input, "wrong error message. Expected '%s', got '%s'\n", expected_e.c_str(), evaluated->data.e.msg_);
    }
    evaluated->release();
}


void TestFunctionObject::execute()
{
    run_core("fn(x) { x + 2;  };");
}


void TestFunctionObject::run_core(std::string input)
{
    Environment env;

     Lexer l(input);
     Parser p(&l);
     std::unique_ptr<Program> program = p.parseProgram();
#ifdef USEVISITOR
     Evaluator evaluator(program.get(), &env);
     Single *fn = evaluator.eval();
#else
     Single *fn = program->eval(&env);
#endif
    if (!fn->type_) {
        errorf(input, "object  is not Function\n");
        return;
    }

    if (fn->data.f.params_->size() != 1) {
        errorf(input, "function has wrong parameters");
        return;
    }

    const std::vector<Identifier *> &idents = *fn->data.f.params_;
    const std::string &val = idents[0]->value();
    if (val.compare("x")) {
        errorf(input, "parameter is not 'x'. got %s\n", val.c_str());
        return;
    }
    std::string expectedBody("(x + 2)");
    const std::string &b = fn->data.f.body_->asString();
    if (fn->data.f.body_->asString() != expectedBody) {
        errorf(input, "body is not %s. got %s\n", expectedBody.c_str(), b.c_str());
        return;
    }

    fn->release();
}


void TestFunctionApplication::run_core(std::string input, int expected)
{
    Environment env;
    Single *ret = eval(input, env);
    testIntegerObject(input, ret, expected);
    ret->release();
}

void TestFunctionApplication::execute()
{
    run_core("let identity = fn(x) { x; }; identity(11);", 11);
    run_core("let identity = fn(x) { return x; }; identity(12);", 12);
    run_core("let ouble = fn(x) { x * 2; }; ouble(7);", 14);
    run_core("let add = fn(x, y) { x + y; }; add(10, 5);", 15);
    run_core("let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20);
    run_core("fn(x) { x; }(90)", 90);
}



void CheckFibonacciTime::execute() {
    //std::string input("let fibonacci = fn(x) { if (x == 0) { 0 } else { if (x == 1) { 1 } else { fibonacci(x - 1) + fibonacci(x - 2); } } }; fibonacci(8);");
    //std::string input("let fibonacci = fn(x) { if (x == 0) { 0 } else { if (x == 1) { 1 } else { fibonacci(x - 1) + fibonacci(x - 2); } } }; fibonacci(25);");
    //std::string input("let fibonacci = fn(x) { if (x == 0) { 0 } else { if (x == 1) { 1 } else { fibonacci(x - 1) + fibonacci(x - 2); } } }; fibonacci(30);");
    std::string input("let fibonacci = fn(x) { if (x == 0) { 0 } else { if (x == 1) { 1 } else { fibonacci(x - 1) + fibonacci(x - 2); } } }; fibonacci(33);");

    //fib(8)  21
    //fib(25) 75025
    //fib(30) 832040
    //fib(33) 3524578
    Environment env;


    Lexer l(input);
    Parser p(&l);
    std::unique_ptr<Program> program = p.parseProgram();
    auto start = high_resolution_clock::now();
#ifdef USEVISITOR
    Evaluator evaluator(program.get(), &env);
    Single *ret = evaluator.eval();
#else
    Single *ret = nullptr;
    ret = program->eval(&env);

#endif
    auto end = high_resolution_clock::now();
    auto time_elapsed = duration_cast<milliseconds>(end - start);
    //testIntegerObject(input, ret, 21);
    //testIntegerObject(input, ret, 75025);
    //testIntegerObject(input, ret, 832040);
    testIntegerObject(input, ret, 3524578);
    if (!is_failed_) {
        std::cout << "fib(33): Time elapsed: " << time_elapsed.count() << " millisec" << std::endl;
    }
    if (ret) ret->release();
}


void TestClosures::execute()
{
    std::string input("let newAdder = fn(x) {"
                            "fn(y) { x + y };"
                        "};"
                        "let addTwo = newAdder(2);"
                        "addTwo(2);");

    Environment env;
    Single *ret = eval(input, env);
    testIntegerObject(input, ret, 4);
    ret->release();
}


void TestStringLiteralExpression::execute()
{
    std::string input("\"hello world\"");
    std::unique_ptr<Program> program = parse(input);

    ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>(program->operator[](0));

    StringLiteral *literal = dynamic_cast<StringLiteral *>(stmt->expression());

    if (!literal) {
        errorf(input, "exp not *ast.StringLiteral\n");
        return;
    }

    if (literal->value().compare("hello world")) {
        errorf(input, "literal.Value not %s. got=%s\n", "\"hello world\"", literal->value().c_str());
        return;
    }
}

void TestStringLiteral::execute()
{
    std::string input("\"Hello World\"");
    Environment env;
    Single *evaluated = eval(input, env);
    if (evaluated->type_ != STRING) {
        errorf(input, "object is not string.got %s\n", object_name[evaluated->type_]);
        return;
    }
    if (strcmp(evaluated->data.s.value_, "Hello World")) {
        errorf(input, "String has wrong value. Expected %s, got %s\n", input.c_str(), evaluated->data.s.value_);
        return;
    }
    evaluated->release();
}


void TestStringConcatenation::execute()
{
    std::string input("\"Hello\" + \" \" + \"World!\"");

    Environment env;
    Single *evaluated = eval(input, env);

    if (evaluated->type_ != STRING) {
        errorf(input, "object is not String.got = %s\n", object_name[evaluated->type_]);
        return;
    }

    if (strcmp(evaluated->data.s.value_, "Hello World!")) {
        errorf(input, "String has wrong value. got=%s, expected %s\n", evaluated->data.s.value_, "Hello World!");
        return;
    }
    evaluated->release();
}


void TestBuiltinFunction::execute()
{
    run_core("len(\"\")", 0);
    run_core("len(\"four\")", 4);
    run_core("len(\"hello world\")", 11);
    run_core("len(1)", "argument to 'len' not supported. got INTEGER\n");
    run_core("len(\"one\", \"two\")", "wrong number of arguments. got: 2, want: 1");
}

void TestBuiltinFunction::run_core(std::string input, std::string expected)
{
    Environment env;
    Single *evaluated = eval(input, env);

    if (evaluated->type_ != ERROR) {
        errorf(input, "object is not ErrorObj");
        return;
    }
    if (strcmp(evaluated->data.e.msg_, expected.c_str())) {
        errorf(input, "wrong error message.\nexpected = %s, got %s\n", expected.c_str(), evaluated->data.e.msg_);
    }

    evaluated->release();
}

void TestBuiltinFunction::run_core(std::string input, int expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testIntegerObject(input, evaluated, expected);
    evaluated->release();
}


void TestParsingArrayLiteral::execute() {

    std::string input("[1, 2 * 2, 3 + 3]");

    std::unique_ptr<Program> program = parse(input);

    ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>(program->operator[](0));

    ArrayLiteral *array = dynamic_cast<ArrayLiteral *>(stmt->expression());

    if (!array) {
        errorf(input, "exp not ArrayLiteral.");
        return;
    }

    if (array->size() != 3) {
        errorf(input, "len(array.Elements) not 3. got: %d", array->size());
        return;
    }

    testIntegerLiteral(input, array->at(0), 1);

    testInfixExpression(input, array->at(1), 2, "*", 2);
    testInfixExpression(input, array->at(2), 3, "+", 3);
}


void TestParsingIndexExpression::execute()
{
    std::string input("myArray[1 + 1]");

    std::unique_ptr<Program> program = parse(input);

    ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>(program->operator[](0));
    IndexExpression *indexExp = dynamic_cast<IndexExpression *>(stmt->expression());
    if (!indexExp) {
        errorf(input, "exp not IndexExpression");
        return;
    }

    if (!testIdentifier(input, indexExp->left(), "myArray")) {
        return;
    }

    if (!testInfixExpression(input, indexExp->index(), 1, "+", 1)) {
        return;
    }
}



void TestArrayLiterals::execute()
{
    std::string input("[1, 2 * 2, 3 + 3]");

    Environment env;
    Single *evaluated = eval(input, env);
    if (evaluated->type_ != ARRAY) {
        errorf(input, "object is not ArrayObj");
        return;
    }

    if (evaluated->data.a.num_ != 3) {
        errorf(input, "array has wrong num of elements. got: %d, expected: 3", evaluated->data.a.num_);
        return;
    }

    testIntegerObject(input, evaluated->data.a.elems_[0], 1);
    testIntegerObject(input, evaluated->data.a.elems_[1], 4);
    testIntegerObject(input, evaluated->data.a.elems_[2], 6);

    evaluated->release();
}

void TestArrayIndexExpressions::execute()
{
    run_core("[1, 2, 3][0]", 1);
    run_core("[1, 2, 3][1]", 2);
    run_core("[1, 2, 3][2]", 3);
    run_core("let i = 0; [1][i];", 1);
    run_core("[1, 2, 3][ 1 + 1 ];", 3);
    run_core("let myArray = [1, 2, 3]; myArray[2];", 3);
    run_core("let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", 6);
    run_core("let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]", 2);
    run_core("[1, 2, 3][3]", nullptr);
    run_core("[1, 2, 3][-1]", nullptr);
}

void TestArrayIndexExpressions::run_core(std::string input, int expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testIntegerObject(input, evaluated, expected);
    evaluated->release();
}

void TestArrayIndexExpressions::run_core(std::string input, void *expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testNullObject(input, evaluated);
    evaluated->release();
}


void TestParsingHashLiteral::execute()
{
    std::string input("{\"one\": 1, \"two\":2, \"three\":3}");

    std::unique_ptr<Program> program = parse(input);

    ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>(program->operator[](0));
    HashLiteral *hash = dynamic_cast<HashLiteral *>(stmt->expression());

    if (!hash) {
        errorf(input, "exp is not HashLiteral");
        return;
    }

    size_t len = hash->size();
    if (len != 3) {
        errorf(input, "hash.pairs_ has wrong length. got: %d\n", len);
        return;
    }
        
    std::unordered_map<std::string, int> expected;
    expected.emplace("one", 1);
    expected.emplace("two", 2);
    expected.emplace("three", 3);

    for (auto &a : *hash) {
        StringLiteral *literal = dynamic_cast<StringLiteral *>(a.first);
        if (!literal) {
            errorf(input, "key is not StringLiteral.");
            return;
        }

        int expectedValue = expected[literal->asString()];
        testIntegerLiteral(input, a.second, expectedValue);
    }
}


void TestParsingEmptyHashLiteral::execute()
{
    std::string input("{}");
    std::unique_ptr<Program> program = parse(input);

    ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>(program->operator[](0));
    HashLiteral *hash = dynamic_cast<HashLiteral *>(stmt->expression());

    if (!hash) {
        errorf(input, "exp is not HashLiteral");
        return;
    }
    
    size_t len = hash->size();
    if (len != 0) {
        errorf(input, "hash.Pairs has wrong length. got: %d", len);
    }
}


void TestParsingHashLiteralWithExpression::execute()
{
    std::string input("{\"one\": 0 + 1, \"two\": 10 - 8, \"three\" : 15/5}");
    std::unique_ptr<Program> program = parse(input);

    ExpressionStatement *stmt = dynamic_cast<ExpressionStatement *>(program->operator[](0));
    HashLiteral *hash = dynamic_cast<HashLiteral *>(stmt->expression());
    if (!hash) {
        errorf(input, "exp is not HashLiteral");
        return;
    }

    size_t len = hash->size();
    if (len != 3) {
        errorf(input, "hash.pairs_ has wrong length");
        return;
    }
    
    
    for (auto &a : *hash) {
        StringLiteral *literal = dynamic_cast<StringLiteral *>(a.first);
        if (!literal) {
            errorf(input, "key is not StringLiteral");
            continue;
        }

        std::string l(literal->asString());
        if (!l.compare("one")) {
            testInfixExpression(input, a.second, 0, "+", 1);
        } else if (!l.compare("two")) {
            testInfixExpression(input, a.second, 10, "-", 8);
        } else if (!l.compare("three")) {
            testInfixExpression(input, a.second, 15, "/", 5);
        } else {
            errorf(input, "No test function for key %s found", l.c_str());
        }
    }
}



void TestHashLiteral::execute()
{
    std::string input("let two = \"two\";"
                    "{"
                        "\"one\": 10 - 9,"
                        "two: 1 + 1,"
                        "\"thr\" + \"ee\": 6 / 2,"
                        "4: 4,"
                        "true: 5,"
                        "false: 6"
                        "}");

    Environment env;
    Single *evaluated = eval(input, env);

    if (evaluated->type_ != HASH) {
        errorf(input, "Eval didn't return Hash");
        return;
    }

    std::unordered_map<Single * , int, HashFn, HashEqFn> expected;
    expected.emplace(Single::alloc("one", STRING), 1);
    expected.emplace(Single::alloc("two", STRING), 2);
    expected.emplace(Single::alloc("three", STRING), 3);
    expected.emplace(Single::alloc(4), 4);
    expected.emplace(&Model::true_o, 5);
    expected.emplace(&Model::false_o, 6);


    HashMap &pair = *evaluated->data.hash.pairs_;
    size_t s = pair.size();
    if (s != expected.size()) {
        errorf(input, "Hash has wrong num of pairs. got : %d, expected: %d", s, expected.size());
        return;
    }

    for (auto &a : expected) {
        Single *value = pair[a.first];
        if (!value) {
            errorf(input, "no pair for given key in Pairs. expected key: %s value: %d",
                    a.first->inspect().c_str(), a.second);
            continue;
        }
        testIntegerObject(input, value, a.second);
    }

    for (auto &a : expected) {
        a.first->release();
    }
    evaluated->release();
}



void TestHashIndexExpressions::run_core(std::string input, int expected)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testIntegerObject(input, evaluated, expected);
    evaluated->release();
}

void TestHashIndexExpressions::run_core(std::string input)
{
    Environment env;
    Single *evaluated = eval(input, env);
    testNullObject(input, evaluated);
    evaluated->release();
}

void TestHashIndexExpressions::execute()
{
    run_core("{\"foo\": 5}[\"foo\"]", 5);
    run_core("{\"foo\": 5}[\"bar\"]");
    run_core("let key = \"foo\"; {\"foo\": 5}[key]", 5);
    run_core("{}[\"foo\"]");
    run_core("{5: 5}[5]", 5);
    run_core("{true: 5}[true]", 5);
    run_core("{false: 5}[false]", 5);
}


