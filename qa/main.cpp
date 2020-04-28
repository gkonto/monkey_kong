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
        tests_.emplace("TestIdentifierExpression", new TestIdentifierExpression);
        tests_.emplace("TestIntegerLiteralExpression", new TestIntegerLiteralExpression);
        tests_.emplace("TestParsingPrefixExpression", new TestParsingPrefixExpression);
        tests_.emplace("TestParsingInfixExpressions", new TestParsingInfixExpressions);
        tests_.emplace("TestOperatorPrecedenceParsing", new TestOperatorPrecedenceParsing);
        tests_.emplace("TestIfExpression", new TestIfExpression);
        tests_.emplace("TestFunctionLiteralParsing", new TestFunctionLiteralParsing);
        tests_.emplace("TestFunctionParametersParsing", new TestFunctionParametersParsing);
        tests_.emplace("TestCallExpressionParsing", new TestCallExpressionParsing);

        //Single creation
        tests_.emplace("TestEvalIntegerExpression", new TestEvalIntegerExpression);
        tests_.emplace("TestEvalBooleanExpression", new TestEvalBooleanExpression);
        tests_.emplace("TestBangOperator", new TestBangOperator);
        tests_.emplace("TestIfElseExpressions", new TestIfElseExpressions);
        tests_.emplace("TestEvalReturnStatements", new TestEvalReturnStatements);
        tests_.emplace("TestErrorHandler", new TestErrorHandler);
        tests_.emplace("TestFunctionObject", new TestFunctionObject);
        tests_.emplace("TestFunctionApplication", new TestFunctionApplication);
        //tests_.emplace("CheckFibonacciTime", new CheckFibonacciTime);
        tests_.emplace("TestClosures", new TestClosures);
        tests_.emplace("TestStringLiteralExpression", new TestStringLiteralExpression);
        tests_.emplace("TestStringLiteral", new TestStringLiteral);
        tests_.emplace("TestStringConcatenation", new TestStringConcatenation);
        tests_.emplace("TestBuiltinFunction", new TestBuiltinFunction);
        tests_.emplace("TestParsingArrayLiteral", new TestParsingArrayLiteral);
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
        std::cout << "[+] " << t.first << ": " << std::flush;
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
