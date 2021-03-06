#ifndef TEST_HPP
#define TEST_HPP

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "../ast.hpp"

class Token;
class Node;
class Program;
struct Object;
struct Test;
class Environment;

struct Test
{
    Test(std::string name) : name_(name) {}
    virtual ~Test(){};
    Object *eval(const std::string &input, Environment &env);
    std::string report_errors() const;
    virtual void execute() = 0;
    void errorf(std::string input_case, const char *fmt, ...);
    void msgf(std::string input_case, const char *fmt, ...);
    std::unique_ptr<Program> parse(const std::string &input);

    bool testLiteralExpression(const std::string &input, Node *exp, bool expected);
    bool testLiteralExpression(const std::string &input, Node *exp, int expected);
    bool testLiteralExpression(const std::string &input, Node *exp, std::string &expected);
    bool testLiteralExpression(const std::string &input, Node *exp, const char *expected);
    bool testIntegerLiteral(const std::string &input, Node *exp, int expected);
    bool testBooleanLiteral(const std::string &input, Node *exp, bool expected);
    bool testIdentifier(const std::string &input, Node *exp, const std::string &expected);
    template <typename T>
    bool testInfixExpression(const std::string &input, Node *exp, T lhs, const std::string &op, T rhs);

    bool testIntegerObject(const std::string &input, Object *obj, int expected);
    bool testBooleanObject(const std::string &input, Object *obj, bool expected);
    bool testNullObject(const std::string &input, Object *obj);
    //
    std::vector<std::string> errors_;
    std::string name_;
    bool is_failed_ = false;
};

class TestNextToken : public Test
{
public:
    TestNextToken() : Test("TestNextToken")
    {
    }
    void execute();
    //void run_core(std::string input, std::vector<Token> expec);

private:
    void run_core(std::string input, std::vector<Token> expec);
    std::string got_;
};

template <typename T>
struct LetStatementCase
{
    LetStatementCase(std::string input, std::string expectedIdentifier, T value)
        : input_(input), expectedIdentifier_(expectedIdentifier), expectedValue_(value) {}

    std::string input_;
    std::string expectedIdentifier_;
    T expectedValue_;
};

class TestLetStatements : public Test
{
public:
    TestLetStatements();
    void execute();

private:
    bool testLetStatement(const std::string &input, Node *statement, const std::string &name);
    template <typename T>
    void run_core(LetStatementCase<T> cas);
};

class TestPoolArena : public Test
{
public:
    TestPoolArena() : Test("TestPoolArena") {}
    void execute();

private:
};

class TestReturnStatements : public Test
{
public:
    TestReturnStatements()
        : Test("TestReturnStatements") {}
    void execute();

private:
    void run_core(std::string input, int expected);
};

class TestIdentifierExpression : public Test
{
public:
    TestIdentifierExpression() : Test("TestIdentifierExpression") {}

    void execute();

private:
};

class TestIntegerLiteralExpression : public Test
{
public:
    TestIntegerLiteralExpression() : Test("TestIntegerLiteralExpression") {}

    void execute();

private:
};

template <typename T>
struct PrefixTest
{
    PrefixTest(const std::string &input, const std::string &operat, T value)
        : input_(input), operator_(operat), value_(value)
    {
    }

    std::string input_;
    std::string operator_;
    T value_;
};

class TestParsingPrefixExpression : public Test
{
public:
    TestParsingPrefixExpression() : Test("TestParsingPrefixExpression") {}
    void execute();

private:
    template <typename T>
    void run_core(PrefixTest<T> a);
};

template <typename T, typename C>
struct InfixTest
{
    InfixTest(const std::string &input, T lhs, const std::string &op, C rhs) : input_(input), lhs_(lhs), op_(op), rhs_(rhs) {}

    std::string input_;
    T lhs_;
    std::string op_;
    C rhs_;
};

class TestParsingInfixExpressions : public Test
{
public:
    TestParsingInfixExpressions() : Test("TestParsingInfixExpressions") {}
    void execute();

private:
    template <typename T, typename C>
    void run_core(InfixTest<T, C> a);
};

struct OperatorPrecedence
{
    OperatorPrecedence(const std::string &input, const std::string &expected)
        : input_(input), expected_(expected) {}

    std::string input_;
    std::string expected_;
};

class TestOperatorPrecedenceParsing : public Test
{
public:
    TestOperatorPrecedenceParsing();
    void execute();

private:
    std::vector<OperatorPrecedence> tests_;
};

class TestIfExpression : public Test
{
public:
    TestIfExpression() : Test("TestIfExpression") {}
    void execute();

private:
};

template <typename T>
bool Test::testInfixExpression(const std::string &input, Node *exp, T lhs, const std::string &op, T rhs)
{
    InfixExpression *opExp = dynamic_cast<InfixExpression *>(exp);
    if (!opExp)
    {
        errorf(input, "exp is not InfixExpression\n");
        return false;
    }

    if (!testLiteralExpression(input, opExp->lhs(), lhs))
    {
        return false;
    }

    if (op.compare(tok_names[opExp->op()]))
    {
        errorf(input, "Operator is not %s. got = %s\n", op.c_str(), tok_names[opExp->op()]);
        return false;
    }

    if (!testLiteralExpression(input, opExp->rhs(), rhs))
    {
        return false;
    }

    return true;
}

class TestFunctionLiteralParsing : public Test
{
public:
    TestFunctionLiteralParsing() : Test("TestFunctionLiteralParsing") {}
    void execute();

private:
};

class TestFunctionParametersParsing : public Test
{
public:
    TestFunctionParametersParsing() : Test("TestFunctionParametersParsing") {}
    void execute();

private:
    void run_case(const std::string input, const std::vector<std::string> &expectedParams_);
};

class TestCallExpressionParsing : public Test
{
public:
    //TODO maybe I dont need the name in Test constructor in ANY test
    TestCallExpressionParsing() : Test("TestCallExpressionParsing") {}
    void execute();

private:
};

class TestEvalIntegerExpression : public Test
{
public:
    TestEvalIntegerExpression()
        : Test("TestEvalIntegerExpression") {}
    void execute();

private:
    void run_core(std::string input, int expected);
};

class TestEvalBooleanExpression : public Test
{
public:
    TestEvalBooleanExpression()
        : Test("TestEvalBooleanExpression") {}

    void execute();

private:
    void run_core(std::string input, bool expected);
};

class TestBangOperator : public Test
{
public:
    TestBangOperator()
        : Test("TestBangOperator") {}

    void execute();

private:
    void run_core(std::string input, bool expected);
};

class TestIfElseExpressions : public Test
{
public:
    TestIfElseExpressions()
        : Test("TestIfElseExpressions") {}

    void execute();

private:
    void run_core(std::string input, int expexted);
    void run_core(std::string input);
};

class TestEvalReturnStatements : public Test
{
public:
    TestEvalReturnStatements()
        : Test("TestEvalReturnStatements") {}
    void execute();

private:
    void run_core(std::string input, int expected);
};

class TestErrorHandler : public Test
{
public:
    TestErrorHandler()
        : Test("TestErrorHandler") {}
    void execute();

private:
    void run_core(std::string input, std::string expected);
};

class TestFunctionObject : public Test
{
public:
    TestFunctionObject()
        : Test("TestFunctionObject") {}
    void execute();

private:
    void run_core(std::string input);
};

class TestFunctionApplication : public Test
{
public:
    TestFunctionApplication() : Test("TestFunctionApplication") {}
    void execute();

private:
    void run_core(std::string input, int expected);
};

class CheckFibonacciTime : public Test
{
public:
    CheckFibonacciTime() : Test("CheckFibonacciTime") {}
    void execute();

private:
};

class TestClosures : public Test
{
public:
    TestClosures() : Test("TestClosures") {}
    void execute();

private:
};

struct TestStringLiteralExpression : public Test
{
    TestStringLiteralExpression() : Test("TestStringLiteralExpression") {}
    void execute();
};

struct TestStringLiteral : public Test
{
    TestStringLiteral() : Test("TestStringLiteral") {}
    void execute();
};

struct TestStringConcatenation : public Test
{
    TestStringConcatenation() : Test("TestStringConcatenation") {}
    void execute();
};

class TestBuiltinFunction : public Test
{
public:
    TestBuiltinFunction() : Test("TestBuiltinFunction") {}
    void execute();

private:
    void run_core(std::string input, std::string expected);
    void run_core(std::string input, int expected);
};

struct TestParsingArrayLiteral : public Test
{
    TestParsingArrayLiteral() : Test("TestParsingArrayLiteral") {}
    void execute();
};

struct TestParsingIndexExpression : public Test
{
    TestParsingIndexExpression() : Test("TestParsingIndexExpression") {}
    void execute();
};

struct TestArrayLiterals : public Test
{
    TestArrayLiterals() : Test("TestArrayLiterals") {}
    void execute();
};

class TestArrayIndexExpressions : public Test
{
public:
    TestArrayIndexExpressions() : Test("TestArrayIndexExpressions") {}
    void execute();

private:
    void run_core(std::string input, int expected);
    void run_core(std::string input, void *expected);
};

struct TestParsingHashLiteral : public Test
{
    TestParsingHashLiteral() : Test("TestParsingHashLiteral") {}
    void execute();
};

struct TestParsingEmptyHashLiteral : public Test
{
    TestParsingEmptyHashLiteral()
        : Test("TestParsingEmptyHashLiteral") {}
    void execute();
};

struct TestParsingHashLiteralWithExpression : public Test
{
    TestParsingHashLiteralWithExpression()
        : Test("TestParsingHashLiteralWithExpression") {}
    void execute();
};

struct TestHashLiteral : public Test
{
    TestHashLiteral() : Test("TestHashLiteral") {}
    void execute();
};

class TestHashIndexExpressions : public Test
{
public:
    TestHashIndexExpressions()
        : Test("TestHashIndexExpressions") {}
    void execute();

private:
    void run_core(std::string input, int expected);
    void run_core(std::string);
};

#endif
