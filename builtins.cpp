#include <memory>
#include "builtins.hpp"
#include "object.hpp"
#include "auxiliaries.hpp"
//#include "compiler_symbol_table.hpp"

using namespace std;

static Single *len_b(const std::array<Single *, MAX_ARGS_NUM> &args, size_t args_num)
{
    if (args_num != 1) {
        char buffer[80];
        sprintf(buffer, "wrong number of arguments. got: %d, want: %d", args_num, 1);
        return Single::alloc(buffer);
    }

    Single *f = args[0];
    if (f->type_ == STRING) {
        return Single::alloc(static_cast<int>(strlen(f->data.string.value_)));
    }

    /*
    ArrayObj *arrObj = dynamic_cast<ArrayObj *>(args[0]);
    if (arrObj) {
        return new IntObj(arrObj->size());
    }
    */

    char buffer[80];
    sprintf(buffer, "argument to 'len' not supported. got %s\n", object_name[f->type_]);
    return Single::alloc(buffer);
}

/*
static Object * first_b(const std::vector<Object *> &args)
{
    if (args.size() != 1) {
        return new ErrorObj("wrong number of arguments. got: %d, want: %d\n", args.size(), 1);
    }

    ArrayObj *arrObj = dynamic_cast<ArrayObj *>(args[0]);
    if (!arrObj) {
        return new ErrorObj("argument to 'first' must be ARRAY");
    }

    if (arrObj->size() > 0) {
        return arrObj->operator[](0);
    }

    return &StandardObjects::null_obj;
}


static Object * last_b(const std::vector<Object *> &args)
{
    if (args.size() != 1) {
        return new ErrorObj("wrong number of arguments. got: %d, want: %d\n", args.size(), 1);
    }

    ArrayObj *arrObj = dynamic_cast<ArrayObj *>(args[0]);
    if (!arrObj) {
        return new ErrorObj("argument to 'last' must be ARRAY");
    }

    if (arrObj->size() > 0) {
        return arrObj->operator[](arrObj->size() - 1);
    }

    return &StandardObjects::null_obj;
}


static Object * rest_b(const std::vector<Object *> &args)
{
    if (args.size() != 1) {
        return new ErrorObj("wrong number of arguments. got: %d, want: %d\n", args.size(), 1);
    }

    ArrayObj *arrObj = dynamic_cast<ArrayObj *>(args[0]);
    if (!arrObj) {
        return new ErrorObj("argument to 'rest' must be ARRAY");
    }

    size_t length = arrObj->size();
    if (length > 0) {
        std::vector<Object *>::const_iterator first = arrObj->begin() + 1;
        std::vector<Object *>::const_iterator last = arrObj->end();
        return new ArrayObj(std::vector<Object *>(first, last));
    }

    return &StandardObjects::null_obj;
}


static Object * push_b(const std::vector<Object *> &args)
{
    if (args.size() != 2) {
        return new ErrorObj("wrong number of arguments. got: %d, want: %d\n", args.size(), 2);
    }

    ArrayObj *arrObj = dynamic_cast<ArrayObj *>(args[0]);
    if (!arrObj) {
        return new ErrorObj("argument to 'push' must be ARRAY");
    }

    std::vector<Object *> elems = arrObj->elements();
    elems.emplace_back(args[1]);

    return new ArrayObj(elems);
}
*/


Builtins &Builtins::getInstance()
{
    static Builtins instance;
    return instance;
}


/*
void Builtins::transferBuiltinsTo(class CompSymbolTable *table) const
{
    int i = 0;

    for (const auto &a : builtins_) {
        table->defineBuiltin(i, a.first);
        ++i;
    }
}
*/


Single *Builtins::atIndex(size_t index) const
{
    const auto &iter = std::next(builtins_.cbegin(), index);
    Single *b = iter->second;
    b->retain();
    return b;
}

Builtins::Builtins()
{
    builtins_.emplace("len", Single::alloc(len_b));
    /*
    builtins_.emplace("first", new Single(first_b));
    builtins_.emplace("last", new Single(last_b));
    builtins_.emplace("push", new Single(push_b));
    builtins_.emplace("rest", new Single(rest_b));
    */
}

Builtins::~Builtins()
{
    // DO NOT DEALLOCATE ANYTHING. These are in the pool 
    // and will be deallocated last.
    //for (auto &a : builtins_) {
        //a.second->release();
    //}
}

Single *Builtins::Get(const string &key)
{
    Builtins &instance = Builtins::getInstance();

    const auto &entry = instance.builtins_.find(key);

    if (entry != instance.builtins_.end()) {
        entry->second->retain();
        return entry->second;
    }

    return nullptr;
}
