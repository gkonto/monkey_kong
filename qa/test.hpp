#ifndef TEST_HPP
#define TEST_HPP

#include <map>
#include <string>
#include <vector>

struct Test;

extern std::map<std::string, Test *> tests;

struct Test
{
    Test(std::string name) : name_(name) {}
    std::string report_errors() const;
    virtual void execute() = 0;
    void errorf();

    std::vector<std::string> errors_;
    std::string name_;
    bool is_failed_ = false;
};



#endif
