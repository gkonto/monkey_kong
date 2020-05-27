#include "env.hpp"
#include "object.hpp"
#include "pool.hpp"

std::unique_ptr<Pool<Environment>> EnvPool = nullptr;

void Environment::release() {
    if (--count_ == 0) {
        dealloc(this);
    }
}

//FIXME code cleanup. Messy condition statements
Single *Environment::get(const std::string &key)
{
    const auto &entry = store_.find(key);
    const auto &end = store_.end();
    if (outer_ && entry == end) {
        const auto &e = outer_->get(key);
        return e;
    } else if (entry == end) {
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



