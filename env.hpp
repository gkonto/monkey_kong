#ifndef ENV_HPP
#define ENV_HPP

#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include "pool.hpp"
#include "auxiliaries.hpp"

struct Object;
class Environment;
class Identifier;

extern std::unique_ptr<Pool<Environment>> EnvPool;

class Environment
{
public:
    Environment() = default;
    explicit Environment(Environment *outer) : outer_(outer) {}
    virtual ~Environment();
    //Object *get(const std::string &key);
    Object *set(const std::string &key, Object *entry);
    virtual Object *get(Identifier *key) const;
    Environment *outer() const { return outer_; }
    void retain() { ++count_; }
    void release();
    void reset() { count_ = 0; }

protected:
    Object *get_core(const std::string &key) const;
    int count_ = 1;
    std::map<std::string, Object *> store_;
    Environment *outer_ = nullptr;
};

class FunctionEnvironment : public Environment
{
    public:
        explicit FunctionEnvironment(Environment *outer, size_t num_args, const std::array<Object *, MAX_ARGS_NUM> &args) :
         Environment(outer), args_values_(args.begin(), args.begin() + num_args) {}

        Object *get(Identifier *key) const;

    private:
        std::vector<Object *> args_values_;
};



#endif
