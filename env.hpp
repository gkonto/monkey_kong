#ifndef ENV_HPP
#define ENV_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>
class Single;

class Environment {
    public:
        Environment() = default;
        explicit Environment(Environment *outer) : outer_(outer) {}
        ~Environment();
        Single *get(const std::string &key) const;
        Single *set(const std::string &key, Single *entry);
        Environment *outer() const { return outer_; }
    private:
        std::unordered_map<std::string, Single *> store_;
        std::unordered_set<Single *> singles_;
        Environment *outer_ = nullptr;
};

#endif
