#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <set>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>

// Forward Declarations
class CmdArg;
class Optional;

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
                const std::set<std::string> &opt_vals, 
                CmdArgCb cb);

        void addArgument(const char *option, const char *description, CmdArgCb cb);
        void parse(void *returned);
    private:
        void displayHelpMessage() const;

        std::unordered_map<std::string, class CmdArg *> accepted_args_; // accepted arguments
        std::vector<std::string> input_args_; // user's input arguments
        std::string parser_desc_; //ArgParser's displaying label when --help is selected
};
//~ArgParser


class CmdArg
{
    public:
        explicit CmdArg(const char *option, const char *desc, CmdArgCb cb);
        explicit CmdArg(const char *option, const char *desc, const std::set<std::string> &accepted, CmdArgCb cb);

        ~CmdArg();
        std::string decorate() const;
        bool hasOptions() const;
        const std::string &option() const;
        const std::string &description() const;
        const std::set<std::string> &accepted_vals() const;
        const std::string &default_val() const;
        bool hasCallback() const;
        void exec_cb(const std::vector<std::string> &arg_opts, void *returned) const;
    private:
        std::string arg_name_; // 
        std::string arg_desc_;   // 
        Optional *optional_ = nullptr;

        CmdArgCb cb_;
};



#endif
