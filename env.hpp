#ifndef ENV_HPP
#define ENV_HPP

#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "pool.hpp"

#define MAX_VECTOR_ENTRIES 10

struct Object;
class Environment;

extern std::unique_ptr<Pool<Environment>> EnvPool;
class StoreStrategy
{
public:
    virtual ~StoreStrategy() {}
    virtual Object *get(const std::string &key) = 0;
    virtual void set(const std::string &key, Object *entry) = 0;
    virtual bool isVector() const { return false; }
    virtual void copyTo(StoreStrategy *to) { std::cout << "EnviromentStategy: ERROR ERROR ERROR" << std::endl; }
    virtual void clear(bool release) = 0;
    virtual size_t size() const = 0;
};

class SStrategyMap : public StoreStrategy
{
public:
    Object *get(const std::string &key);
    void set(const std::string &key, Object *entry);
    void clear(bool release);
    size_t size() const { return store_.size(); }

private:
    std::unordered_map<std::string, Object *> store_;
};

class SStrategyMapVector : public StoreStrategy
{
    using KeyEntryP = std::pair<std::string, Object *>;

public:
    SStrategyMapVector()
    {
        store_.reserve(MAX_VECTOR_ENTRIES);
    }

    bool isVector() const { return true; }
    Object *get(const std::string &key);
    void set(const std::string &key, Object *entry);
    void copyTo(StoreStrategy *to);
    void clear(bool release);
    size_t size() const { return store_.size(); }

private:
    std::vector<KeyEntryP> store_;
};

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
    void switchToMapStrategy();

    template <typename... Args>
    static Environment *alloc(Args... args);
    static void dealloc(Environment *env);

private:
    StoreStrategy *m_store_strategy = new SStrategyMap;
    Environment *outer_ = nullptr;
    int count_ = 1;
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
