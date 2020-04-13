#include "argparser.hpp"

int main(int argc, char **argv)
{
    ArgParser a_parser("Interpreter QA mechanism", argc, argv);
    a_parser.addArgument("--help", "Display this information", nullptr);
    a_parser.addArgument("-run", "Select tests to run", nullptr);
    a_parser.parse(nullptr);
}
