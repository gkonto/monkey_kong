#include <string>
#include <sstream>
#include "object.hpp"
#include "env.hpp"

SinglePool *testPool = nullptr;

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


void Single::release() {
    --count_;
    if (count_ == 0) {
        if (this == &Model::false_o || this == &Model::true_o || this == &Model::null_o) {
            return;
        }

        if (type_ == ERROR) {
            free(data.error.msg_);
        } else if (type_ == FUNCTION) {
            data.function.env_->release();
        }
        //Auto prepei na fugei kai na allaksei to next
        SinglePool &p = SinglePool::getInstance();
        p.makeAvailable(this);
    }
}

