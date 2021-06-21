#ifndef ENV_HPP
#define ENV_HPP

#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include "pool.hpp"

struct Object;
class Environment;

extern std::unique_ptr<Pool<Environment>> EnvPool;

class Environment
{
public:
    Environment() = default;
    explicit Environment(Environment *outer) : outer_(outer) {}
    ~Environment();
    Object *get(const std::string &key);
    Object *set(const std::string &key, Object *entry);
    Environment *outer() const { return outer_; }
    void retain() { ++count_; }
    void release();
    void reset() { count_ = 0; }

    template <typename... Args>
    static Environment *alloc(Args... args);
    static void dealloc(Environment *env);

private:
    int count_ = 1;
    std::map<std::string, Object *> store_;
    Environment *outer_ = nullptr;
};

template <typename... Args>
Environment *Environment::alloc(Args... args)
{
    if (!EnvPool)
    {
        EnvPool = std::make_unique<Pool<Environment>>(10);
    }
    Environment *newEnv = EnvPool->alloc(std::forward<Args>(args)...);
    newEnv->reset();
    newEnv->retain();
    return newEnv;
}

#endif
