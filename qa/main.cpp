#include <iostream>
#include "../argparser.hpp"
#include "../auxiliaries.hpp"
#include "test.hpp"

struct QAUserArgs
{
};

int main(int argc, char **argv)
{
    QAUserArgs u_args;
    ArgParser a_parser("Interpreter QA mechanism", argc, argv);
    a_parser.addArgument("--help", "Display this information", nullptr);
    a_parser.parse(nullptr);

    for (auto &t : tests) {
        std::cout << "[+] " << t.first << ": ";
        t.second->execute();
        if (t.second->is_failed_) {
            std::cout << RED << "FAILED" << RESET << std::endl;
            std::cout << t.second->report_errors() << std::endl;
            break;
        } else {
            std::cout << GREEN << "FAILED" << RESET << std::endl;
        }
    }
}
