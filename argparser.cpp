#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "argparser.hpp"

using namespace std;

struct Optional
{
    explicit Optional(string default_v, const set<string> &accepted)
        : default_val_(default_v), accepted_vals_(accepted) {}

    string default_val_;
    set<string> accepted_vals_;
}; 
//~Optional


class CmdArg
{
    public:
        explicit CmdArg(const char *option, const char *desc, CmdArgCb cb) 
            : option_(option), desc_(desc), cb_(cb) {}

        explicit CmdArg(const char *option, const char *desc, const char *default_v, const set<string> &accepted, CmdArgCb cb)
                    : option_(option), desc_(desc), optional_(new Optional(default_v, accepted)), cb_(cb) {}

        string decorate() const;
        bool hasOptions() const { return optional_ != nullptr; }
        const string &option() const { return option_; }
        const string &description() const { return desc_; }
        const set<string> &accepted_vals() const { return optional_->accepted_vals_; }
        const string &default_val() const { return optional_->default_val_; }
        bool hasCallback() const { return cb_ != nullptr; }
        void exec_cb(const vector<string> &arg_opts, void *returned) const
                    { cb_(*this, arg_opts, returned); };
    private:
        string option_; // 
        string desc_;   // 
        Optional *optional_ = nullptr;

        CmdArgCb cb_;
};


ArgParser::ArgParser(string name, int argc, char **argv)
    : name_(name)
{
    user_args_.reserve(argc);
    for (int i = 0; i < argc; i++) {
        user_args_.emplace_back(argv[i]);
    }
}


ArgParser::~ArgParser()
{
    for (auto &a : args_) {
        delete a.second;
    }
}


void ArgParser::addArgument(const char *option, const char *desc, const char *default_val, 
        const set<string> &opt_vals, CmdArgCb cb)
{
    args_.emplace(option, new CmdArg(option, desc, default_val, opt_vals, cb));
}


void ArgParser::addArgument(const char *option, const char *desc, CmdArgCb cb)
{
    args_.emplace(option, new CmdArg(option, desc, cb));
}


void ArgParser::displayHelpMessage() const
{
    cout << name_ << endl;
    cout << "Options: " << endl;
    for (auto &opt : args_) {
        cout << " " << opt.second->decorate() << endl;
    }
}


void ArgParser::parse(void *returned)
{
    auto it = find(user_args_.begin(), user_args_.end(), "--help");

    if (it != user_args_.end()) {
        displayHelpMessage();
        exit(1);        
    }

    for (size_t i = 0; i < user_args_.size(); i++) {
        auto it = args_.find(user_args_[i]);

        if (it == args_.end()) continue;

        const CmdArg *arg = it->second;

        if (arg->hasOptions()) {
            vector<string> arg_opts;
            while (args_.find(user_args_[++i]) != args_.end()) {
                arg_opts.emplace_back(user_args_[i]);
            }
            if (arg->hasCallback()) {
                arg->exec_cb(arg_opts, returned);
            }
        }
    }
}


string CmdArg::decorate() const
{
    stringstream ss;
    if (!hasOptions()) {
        ss << left << setw(30) << option_ << desc_;
        return ss.str();
    }

    string first(option());
    first.append(" {");
    for (auto &o : optional_->accepted_vals_) {
        first.append(o);
        first.append("|");
    }
    first.pop_back();
    first.append("}");

    string second(description());
    second.append(" ");
    second.append("Default value: ");
    second.append(optional_->default_val_);

    ss << left << setw(30) << first << second;

    return ss.str();
}
//Cmd::decorate

