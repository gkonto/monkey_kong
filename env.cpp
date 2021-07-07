#include "env.hpp"
#include "object.hpp"
#include "pool.hpp"
#include "ast.hpp"

std::unique_ptr<Pool<Environment>> EnvPool = nullptr;

void Environment::release()
{
    if (--count_ == 0)
    {
        delete this;
    }
}

//FIXME code cleanup. Messy condition statements
Object *Environment::get_core(const std::string &key) const
{
    const auto &entry = store_.find(key);
    const auto &end = store_.end();
    if (outer_ && entry == end)
    {
        const auto &e = outer_->get_core(key);
        return e;
    }
    else if (entry == end)
    {
        return nullptr;
    }

    return entry->second;
}

Object *Environment::get(Identifier *iden) const
{
    return get_core(iden->value());
}

Object *FunctionEnvironment::get(Identifier *key) const
{
    int index = key->index();
    if (index != -1) {
        return args_values_[index];
    } else {
        return get_core(key->value());
    }
}

Object *Environment::set(const std::string &key, Object *entry)
{
    store_[key] = entry;
    entry->retain();

    return entry;
}

Environment::~Environment()
{
    for (auto &a : store_)
    {
        a.second->release();
    }
}
