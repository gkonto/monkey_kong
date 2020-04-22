#include <string>
#include <sstream>
#include "object.hpp"
#include "env.hpp"

SinglePool *testPool = nullptr;

std::unique_ptr<Pool<Single>> SingPool = nullptr;

namespace Model {
    Single true_o(true);
    Single false_o(false);
    Single null_o;
}

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

        Single::dealloc(this);
    }
}

void Single::dealloc(Single *o) {
    if (!o) return;
    assert(SingPool);
    if (SingPool) {
        SingPool->free(o);
    }
}

