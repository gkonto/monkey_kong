#include <iostream>
#include <map>
#include "../argparser.hpp"
#include "../auxiliaries.hpp"
#include "../token.hpp"
#include "test.hpp"

using namespace std;



struct Tests
{
    Tests()
    {
        tests_.emplace("TestPoolArena", new TestPoolArena);
        tests_.emplace("TestNextToken", new TestNextToken);
        tests_.emplace("TestLetStatements", new TestLetStatements);
        tests_.emplace("TestReturnStatements", new TestReturnStatements);
    }

    ~Tests()
    {
        for (auto &t : tests_) {
            delete t.second;
        }
    }



    std::string list()
    {
        std::string ret("Select specific tests. Available tests:\n");
        for (auto &t : tests_) {
            ret.append("\t\t\t\t\t" + t.first + "\n");
        }
        return ret;
    }

    set<string> test_names()
    {
        set<string> names;
        for (auto &t : tests_) {
            names.emplace(t.first);
        }

        return names;
    }

    map<string, Test *>::const_iterator begin() const
    {
        return tests_.cbegin();
    }

    map<string, Test *>::const_iterator end() const
    {
        return tests_.cend();
    }

    map<string, Test *> tests_;
};

struct QAArgs
{
    QAArgs(const Tests &tests)
        : all_tests_(tests)
    {
        for (const auto &t : all_tests_) {
            test_to_run_.emplace(t);
        }
    }

    const Tests  &all_tests_;
    map<string, Test *> test_to_run_;
};


static void gatherTests(const class CmdArg &ard, const std::vector<std::string> &input_options, void *data)
{
    QAArgs *qa_opts = static_cast<QAArgs *>(data);
    if (!input_options.empty()) {
        qa_opts->test_to_run_.clear();
    }
    for (auto &opt : input_options) {
        const auto &it = qa_opts->all_tests_.tests_.find(opt);
        if (it != qa_opts->all_tests_.tests_.end()) {
            qa_opts->test_to_run_.emplace(it->first, it->second);
        }
        if (!opt.compare("all")) {
            qa_opts->test_to_run_.clear();
            for (auto &t : qa_opts->all_tests_.tests_) {
                qa_opts->test_to_run_.emplace(t.first, t.second);
            }
            break;
        }
    }
}

int main(int argc, char **argv)
{
    Tests tt;
    QAArgs data(tt);
    ArgParser a_parser("Interpreter QA mechanism", argc, argv);
    a_parser.addArgument("-run", tt.list().c_str(), tt.test_names(),  gatherTests);
    a_parser.addArgument("--help", "Display this information", nullptr);
    a_parser.parse(&data);

    for (auto &t : data.test_to_run_) {
        std::cout << "[+] " << t.first << ": ";
        t.second->execute();
        TokenPool.reset();
        if (t.second->is_failed_) {
            std::cout << RED << "FAILED" << RESET << std::endl;
            std::cout << t.second->report_errors() << std::endl;
            break;
        } else {
            std::cout << GREEN << "PASSED" << RESET << std::endl;
        }
    }
}
