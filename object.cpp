#include <string>
#include <sstream>
#include "object.hpp"

namespace Model {
    Single true_o(true);
    Single false_o(false);
    Single null_o;
}

std::string Integer::inspect() const {
    std::stringstream ss;
    ss << value_;
    return ss.str();
}

std::string Bool::inspect() const {
    std::stringstream ss;
    ss << value_;
    return ss.str();
}




