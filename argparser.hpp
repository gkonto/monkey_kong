#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <set>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>

// Forward Declarations
class ReplOptions;
class CmdArg;

#define OPT_NOT_ACTIVE "OPT_NOT_ACTIVE"
#define OPT_ACTIVE "OPT_ACTIVE"

using CmdArgCb = std::function<void(const class CmdArg &, const std::vector<std::string> &, void *returned)>;

class ArgParser
{
    public:
        explicit ArgParser(std::string name, int argc, char **argv);
        ~ArgParser();
        void addArgument(const char *option, 
                const char *description,
                const char *default_val,
                const std::set<std::string> &opt_vals, 
                CmdArgCb cb);

        void addArgument(const char *option, const char *description, CmdArgCb cb);
        void parse(void *returned);
    private:
        void displayHelpMessage() const;

        std::unordered_map<std::string, class CmdArg *> args_; // accepted arguments
        std::vector<std::string> user_args_; // user's input arguments
        std::string name_; //ArgParser's displaying label when --help is selected
};
//~ArgParser


#endif
