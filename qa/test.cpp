#include <iostream>
#include <sstream>
#include "test.hpp"

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


void Test::errorf()
{
    

}

