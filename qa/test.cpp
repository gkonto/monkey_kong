#include <iostream>
#include <sstream>
#include <string>
#include <numeric>
#include <stdarg.h>
#include <memory>
#include "test.hpp"
#include "../token.hpp"
#include "../visitor.hpp"
#include "../lexer.hpp"
#include "../ast.hpp"
#include "../parser.hpp"
#include "../pool.hpp"
#include "../object.hpp"

using namespace std;

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

     if (exp->op().compare(a.op_)) {
         errorf(a.input_, "Operator is not %s. got = %s\n", a.op_.c_str(), exp->op().c_str());
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
     /*
     tests_.emplace_back(OP("a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"));
     tests_.emplace_back(OP("add(a * b[2], b[1], 2 * [1, 2][1])", "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"));
     */
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


 bool Test::testIntegerObject(const std::string &input, Object *obj, int expected)
 {
     if (!obj) {
         errorf(input, "Evaluated obj is nullptr\n");
         return false;
     }

     Integer *result = dynamic_cast<Integer *>(obj);

     if (!result) {
         errorf(input, "object is not Integer.got %s\n", object_name[obj->type()]);
         return false;
     }

     if (result->value() != expected) {
         errorf(input, "object value was wrong %d. got %d\n", expected, result->value());
         return false;
     }

     return true;
 }


Object *Test::eval(const std::string &input)
{
     Lexer l(input);
     Parser p(&l);
     std::unique_ptr<Program> program = p.parseProgram();

     Evaluator evaluator(program.get());
     Object *ret = evaluator.eval();
     return ret;
}

void TestEvalIntegerExpression::run_core(std::string input, int expected)
 {
     Object *evaluated = eval(input);
     testIntegerObject(input, evaluated, expected);
     delete evaluated;
 }

 void TestEvalIntegerExpression::execute()
 {
     run_core("5", 5);
     run_core("10", 10);
     /*
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
     */
 }

