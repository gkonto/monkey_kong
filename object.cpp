#include <string>
#include <sstream>
#include "object.hpp"


std::string Integer::inspect() const {
    std::stringstream ss;
    ss << value_;
    return ss.str();
}

std::string Boolean::inspect() const {
    std::stringstream ss;
    ss << value_;
    return ss.str();
}



