#ifndef TEST_HPP
#define TEST_HPP

#include <map>
#include <string>
#include <vector>

class Token;
struct Test;

struct Test
{
    Test(std::string name) : name_(name) {}
    virtual ~Test() {};
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
        //void run_core(std::string input, std::vector<Token> expec);

    private:
        void run_core(std::string input, std::vector<Token> expec);
        std::string got_;
};



#endif
