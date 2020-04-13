#include "../argparser.hpp"

struct QAUserArgs
{
};

int main(int argc, char **argv)
{
    QAUserArgs u_args;
    ArgParser a_parser("Interpreter QA mechanism", argc, argv);
    a_parser.addArgument("--help", "Display this information", nullptr);

    a_parser.parse(nullptr);
}
