#include <string>
#include <sstream>
#include "object.hpp"
#include "env.hpp"

namespace Model {
    Single true_o(true);
    Single false_o(false);
    Single null_o;
}

/*
void DeleteSingle(Single *p)
{
    if (p == &Model::false_o || p == &Model::true_o || p == &Model::null_o) {
        return;
    }
    delete (p);
}
*/


