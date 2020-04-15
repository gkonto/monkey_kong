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
         TestIdentifierExpression() :
             Test("TestIdentifierExpression") {}

         void execute();
     private:
 };


class TestIntegerLiteralExpression : public Test
 {
     public:
         TestIntegerLiteralExpression() :
             Test("TestIntegerLiteralExpression") {}

         void execute();
     private:
 };



 template <typename T>
 struct PrefixTest
 {
     PrefixTest(const std::string &input, const std::string &operat, T value)
         : input_(input), operator_(operat), value_(value)
     {}

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
         template<typename T>
         void run_core(PrefixTest<T> a);
 
 };

template<typename T, typename C>
 struct InfixTest
 {
     InfixTest(const std::string &input, T lhs, const std::string &op, C rhs) :
         input_(input), lhs_(lhs), op_(op), rhs_(rhs) {}
 
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
         template<typename T, typename C>
         void run_core(InfixTest<T, C> a);
 };







#endif
