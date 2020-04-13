#ifndef TEST_HPP
#define TEST_HPP

#include <map>
#include <string>
#include <vector>

class Token;
struct Test;

extern std::map<std::string, Test *> tests;

struct Test
{
    Test(std::string name) : name_(name) {}
    std::string report_errors() const;
    virtual void execute() = 0;
    void errorf(std::string input_case, const char *fmt, ...);

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

    private:
        void run_core(std::string input, std::vector<Token> expec);
        std::string got_;
};



#endif
