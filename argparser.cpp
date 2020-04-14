#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "argparser.hpp"

using namespace std;

struct Optional
{
    explicit Optional(const set<string> &accepted)
        : accepted_vals_(accepted) {}

    set<string> accepted_vals_;
}; 
//~Optional


CmdArg::CmdArg(const char *option, const char *desc, CmdArgCb cb)
             : arg_name_(option), arg_desc_(desc), cb_(cb) 
{}

CmdArg::CmdArg(const char *option, const char *desc, const set<string> &accepted, CmdArgCb cb)
                    : arg_name_(option), arg_desc_(desc), optional_(new Optional(accepted)), cb_(cb)
{
}

CmdArg::~CmdArg()
{
    if (hasOptions()) {
        delete optional_;
    }
}

bool CmdArg::hasOptions() const 
{
    return optional_ != nullptr; 
}

const string &CmdArg::option() const 
{ 
    return arg_name_; 
}

const string &CmdArg::description() const 
{ 
    return arg_desc_; 
}

const set<string> &CmdArg::accepted_vals() const 
{
    return optional_->accepted_vals_; 
}


bool CmdArg::hasCallback() const 
{
    return cb_ != nullptr; 
}


void CmdArg::exec_cb(const vector<string> &arg_opts, void *returned) const
{
    if (!cb_) {
        std::cout << "Argument: " << description() << "Missing callback!" << std::endl;
    }
    cb_(*this, arg_opts, returned); 
}


ArgParser::ArgParser(string name, int argc, char **argv)
    : parser_desc_(name)
{
    input_args_.reserve(argc);
    for (int i = 0; i < argc; i++) {
        input_args_.emplace_back(argv[i]);
    }
}


ArgParser::~ArgParser()
{
    for (auto &a : accepted_args_) {
        delete a.second;
    }
}


void ArgParser::addArgument(const char *option, const char *desc, 
        const set<string> &opt_vals, CmdArgCb cb)
{
    accepted_args_.emplace(option, new CmdArg(option, desc, opt_vals, cb));
}


void ArgParser::addArgument(const char *option, const char *desc, CmdArgCb cb)
{
    accepted_args_.emplace(option, new CmdArg(option, desc, cb));
}


void ArgParser::displayHelpMessage() const
{
    cout << parser_desc_ << endl;
    cout << "Options: " << endl;
    for (auto &opt : accepted_args_) {
        cout << " " << opt.second->decorate() << endl;
    }
}


void ArgParser::parse(void *returned)
{
    auto it = find(input_args_.begin(), input_args_.end(), "--help");

    if (it != input_args_.end()) {
        displayHelpMessage();
        exit(1);        
    }
    
    for (size_t i = 0; i < input_args_.size(); i++) {
        auto it = accepted_args_.find(input_args_[i]);

        if (it == accepted_args_.end()) continue;

        const CmdArg *arg = it->second;

        vector<string> arg_opts;

        if (arg->hasOptions()) {
            const set<string> &accepted_opts = arg->accepted_vals();
            while (++i < input_args_.size() && accepted_opts.find(input_args_[i]) != accepted_opts.end()) {
                arg_opts.emplace_back(input_args_[i]);
            }
            if (arg->hasCallback()) {
                arg->exec_cb(arg_opts, returned);
            }
        } else {
            arg->exec_cb(arg_opts, returned);
        }
    }
}


string CmdArg::decorate() const
{
    stringstream ss;
    if (!hasOptions()) {
        ss << left << setw(30) << arg_name_ << arg_desc_;
        return ss.str();
    }

    string first(option());
    string second(description());
    second.append(" ");

    ss << left << setw(30) << first << second;

    return ss.str();
}
//Cmd::decorate

