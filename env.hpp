#ifndef ENV_HPP
#define ENV_HPP

#include <unordered_map>
#include <string>
class Single;

class Environment {
    public:
        Single *get(const std::string &key) const;
        Single *set(const std::string &key, Single *entry);
    private:
        std::unordered_map<std::string, Single *> store_;
        Environment *outer_ = nullptr;
};

#endif
