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


Single *Environment::set(const std::string &key, Single *entry)
{
    /*
    using namespace StandardObjects;
    Single *cl = entry->clone();

    auto it = store_.find(key);
    if (it != store_.end()) {
        Single *o = it->second;
        if (o && o != &false_obj && o != &true_obj && o != &null_obj) {
            delete it->second;
        }
    }
    */
    store_[key] = entry;

    return entry;
}


