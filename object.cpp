#include <string>
#include <sstream>
#include "object.hpp"
#include "ast.hpp"
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

        if (type_ == ERROR || type_ == STRING) {
            free(data.error.msg_);
        } else if (type_ == FUNCTION) {
            data.function.env_->release();
        } else if (type_ == ARRAY) {
            if (data.array.num_) {
                delete []data.array.elems_;
            }
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

std::string Single::inspectFunction() const {
    std::string ret;
    ret.append("fn");
    ret.append("(");

    size_t i = 1;
    size_t num = data.function.parameters_->size();

    for (auto &a : *data.function.parameters_) {
        ret.append(a->asString());
        if (i != num) {
            i++;
            ret.append(", ");
        }
    }
    ret.append(") {\n");
    ret.append(data.function.body_->asString());
    ret.append("\n}");

    return ret;
}

std::string Single::inspectArray() const {
    std::string ret;
    ret.append("[");

    //TODO refactoring
    size_t i = 1;
    size_t num = data.array.num_;

    for (int k = 0; k < data.array.num_; ++k) {

        ret.append(data.array.elems_[k]->inspect());
        if (i != num) {
            i++;
            ret.append(", ");
        }
    }
    ret.append("]");

    return ret;
}


