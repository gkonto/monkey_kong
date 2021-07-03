#include "env.hpp"
#include "object.hpp"
#include "pool.hpp"

std::unique_ptr<Pool<Environment>> EnvPool = nullptr;

Object *SStrategyMap::get(const std::string &key)
{
    auto entry = store_.find(key);
    return entry == store_.end() ? nullptr : entry->second;
}

void SStrategyMap::set(const std::string &key, Object *entry)
{
    store_[key] = entry;
}

Object *SStrategyMapVector::get(const std::string &key)
{
    for (auto entry : store_)
    {
        if (entry.first == key)
        {
            return entry.second;
        }
    }
    return nullptr;
}

void SStrategyMapVector::set(const std::string &key, Object *entry)
{
    for (auto &p : store_)
    {
        if (p.first == key)
        {
            p.second = entry;
        }
    }
    store_.emplace_back(key, entry);
}

void SStrategyMapVector::copyTo(StoreStrategy *to)
{
    for (auto p : store_)
    {
        to->set(p.first, p.second);
    }
}

void SStrategyMapVector::clear(bool release)
{
    if (release)
    {
        for (auto &a : store_)
        {
            a.second->release();
        }
        store_.clear();
    }
}

void SStrategyMap::clear(bool release)
{
    if (release)
    {
        for (auto &a : store_)
        {
            a.second->release();
        }
    }
    store_.clear();
}

void Environment::release()
{
    if (--count_ == 0)
    {
        dealloc(this);
    }
}

//FIXME code cleanup. Messy condition statements
Object *Environment::get(const std::string &key)
{
    Object *entry = m_store_strategy->get(key);

    if (outer_ && !entry)
    {
        const auto &e = outer_->get(key);
        return e;
    }
    else if (!entry)
    {
        return nullptr;
    }

    return entry;
}

void Environment::switchToMapStrategy()
{
    SStrategyMap *new_strat = new SStrategyMap;
    m_store_strategy->copyTo(new_strat);
    m_store_strategy->clear(false);
    m_store_strategy = new_strat;
}

Object *Environment::set(const std::string &key, Object *entry)
{
    if (m_store_strategy->size() > MAX_VECTOR_ENTRIES && m_store_strategy->isVector())
    {
        // Change container. Use map.
        switchToMapStrategy();
    }
    m_store_strategy->set(key, entry);
    entry->retain();

    return entry;
}

Environment::~Environment()
{
    m_store_strategy->clear(1);
    delete m_store_strategy;
}

void Environment::dealloc(Environment *env)
{
    if (!env)
        return;
    assert(EnvPool);
    if (EnvPool)
    {
        EnvPool->free(env);
    }
}
