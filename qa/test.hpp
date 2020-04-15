#ifndef TEST_HPP
#define TEST_HPP

#include <map>
#include <string>
#include <vector>
#include <memory>

class Token;
class Node;
class Program;
struct Test;

struct Test
{
    Test(std::string name) : name_(name) {}
    virtual ~Test() {};
    std::string report_errors() const;
    virtual void execute() = 0;
    void errorf(std::string input_case, const char *fmt, ...);
    std::unique_ptr<Program> parse(const std::string &input);

    bool testLiteralExpression(const std::string &input, Node *exp, bool expected);
    bool testLiteralExpression(const std::string &input, Node *exp, int expected);
    bool testLiteralExpression(const std::string &input, Node *exp, std::string &expected);

    bool testIntegerLiteral(const std::string &input, Node *exp, int expected);
    bool testBooleanLiteral(const std::string &input, Node *exp, bool expected);
    bool testIdentifier(const std::string &input, Node *exp, const std::string &expected);
  //
    std::vector<std::string> errors_;
    std::string name_;
    bool is_failed_ = false;
};


class TestNextToken : public Test
{
    public:
        TestNextToken() : Test("TestNextToken")
        {}
        void execute();
        //void run_core(std::string input, std::vector<Token> expec);

    private:
        void run_core(std::string input, std::vector<Token> expec);
        std::string got_;
};


template<typename T>
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
        template<typename T>
        void run_core(LetStatementCase<T> cas);
};


class TestPoolArena : public Test
{
    public:
        TestPoolArena() : Test("TestPoolArena") {}
        void execute();
    private:
};






#endif
