#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <map>
#include <string>
#include <memory>
#include <array>
#include "auxiliaries.hpp"

struct Single;
class CompSymbolTable;

class Builtins
{
    public:
        using Function = Single *(*)(const std::array<Single *, MAX_ARGS_NUM> &args, size_t args_num);
        using container = std::map<std::string, Single *>;

        static Single *Get(const std::string &key);
        static Builtins &getInstance();
        Single *atIndex(size_t index) const;
    private:
        explicit Builtins();
        container builtins_;
};


#endif
