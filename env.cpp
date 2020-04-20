#include "env.hpp"
#include "object.hpp"

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

/*
void Environment::erase(Single *entity) {
    if (entity) {
        singles_.erase(entity);
    }
}
*/

Single *Environment::set(const std::string &key, Single *entry)
{
    store_[key] = entry;
    entry->retain();
    //singles_.emplace(entry);

    return entry;
}


Environment::~Environment() {
    for (auto &a : store_) {
        a.second->release();
        //a->release();
        //delete a;
    }
    //if (outer_) delete outer_;
}


