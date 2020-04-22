#include "env.hpp"
#include "object.hpp"
#include "pool.hpp"

std::unique_ptr<Pool<Environment>> EnvPool = nullptr;

//FIXME code cleanup. Messy condition statements
Single *Environment::get(const std::string &key) const
{
    auto entry = store_.find(key);

    if (entry == store_.end() && outer_) {
        auto e = outer_->get(key);
        return e;
    } else if (entry == store_.end()) {
        return nullptr;
    }

    return entry->second;
}

Single *Environment::set(const std::string &key, Single *entry)
{
    store_[key] = entry;
    entry->retain();

    return entry;
}


Environment::~Environment() {
    for (auto &a : store_) {
        a.second->release();
    }
}

void Environment::dealloc(Environment *env) {
    if (!env) return;
    assert(EnvPool);
    if (EnvPool) {
        EnvPool->free(env);
    }
}



