#include <sstream>
#include <iostream>
#include <memory>
#include <chrono>

#include "argparser.hpp"
#include "repl.hpp"
#include "ast.hpp"
#include "visitor.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "auxiliaries.hpp"

using namespace std;
using namespace std::chrono;

void Repl::greeting() const
{
    std::stringstream ss;
    cout << BOLD << "Hello" << "! ";
    cout << "This is the Monkey Programming language " << CYAN << type() << RESET << "." << std::endl;
    cout << BOLD << "Feel free to type in commands." << RESET << std::endl;
}

void Repl::start(bool calc_t)
{
    greeting();

    while (true) {
        // Get input
        string input;
        cout << prompt_;
        getline(cin, input);
        // Produce program nodes
        auto start = high_resolution_clock::now();
        Lexer lex(input);
        Parser p(&lex);
        std::unique_ptr<Program> prog = p.parseProgram();

        Evaluator evaluator(prog.get());
        Object *obj = evaluator.eval();

        auto end = high_resolution_clock::now();

        if (obj) {
            std::cout << obj->inspect() << std::endl;
            delete obj;
        }

        if (calc_t) {
            auto time_elapsed = duration_cast<milliseconds>(end - start);
            cout << BOLD << endl << "time elapsed: " << time_elapsed.count() << " millisec" << RESET << std::endl;
        }
    }
}


InterpreterRepl::InterpreterRepl()
{
}


struct ReplOptions
{
    bool   time_   = false;
}; 
// ~ReplOptions


static void displayElapsedTimeArg(const CmdArg &arg, const vector<string> &user_option, void *r_opts)
{
    ReplOptions *ops = static_cast<ReplOptions *>(r_opts); 
    ops->time_ = true;
}


int main(int argc, char **argv)
{
    ReplOptions r_opts;
    ArgParser a_parser("Monkey Programming Language", argc, argv);
    a_parser.addArgument("--help", "Display this information", nullptr);
    a_parser.addArgument("-s", "Display elapsed time", displayElapsedTimeArg);
    a_parser.parse(&r_opts);

    std::unique_ptr<InterpreterRepl> repl = make_unique<InterpreterRepl>();
    repl->start(r_opts.time_);

}
