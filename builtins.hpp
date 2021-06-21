#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <map>
#include <string>
#include <memory>
#include <array>
#include "auxiliaries.hpp"

struct Object;
class CompSymbolTable;

class Builtins
{
public:
    using Function = Object *(*)(const std::array<Object *, MAX_ARGS_NUM> &args, size_t args_num);
    using container = std::map<std::string, Object *>;

    static Object *Get(const std::string &key);
    static Builtins &getInstance();
    Object *atIndex(size_t index) const;

private:
    explicit Builtins();
    container builtins_;
};

#endif
