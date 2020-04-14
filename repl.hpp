#ifndef REPL_HPP
#define REPL_HPP

#include <string>

class Repl
{
    public:
        virtual ~Repl() {}
        virtual const char *type() const = 0;

        void start(bool calc_t);
        void greeting() const;
    protected:
        explicit Repl() {};
        const std::string prompt_ = ">> ";
};


class InterpreterRepl : public Repl
{
    public:
        InterpreterRepl();
        const char *type() const { return "Interpreter"; }

    private:
};


#endif
