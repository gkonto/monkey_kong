#include <memory>
#include "builtins.hpp"
#include "object.hpp"
#include "auxiliaries.hpp"

using namespace std;

static Object *argNumCheck(int got, int expected)
{
    if (got != expected)
    {
        char buffer[80];
        sprintf(buffer, "wrong number of arguments. got: %d, want: %d", got, expected);
        return Object::alloc(buffer);
    }

    return nullptr;
}

static Object *len_b(const std::array<Object *, MAX_ARGS_NUM> &args, size_t args_num)
{
    Object *err = argNumCheck(args_num, 1);
    if (err)
        return err;

    Object *f = args[0];
    if (f->type_ == STRING)
    {
        return Object::alloc(static_cast<int>(strlen(f->data.s.value_)));
    }
    else if (f->type_ == ARRAY)
    {
        return Object::alloc(static_cast<int>(args_num));
    }

    char buffer[80];
    sprintf(buffer, "argument to 'len' not supported. got %s\n", object_name[f->type_]);
    return Object::alloc(buffer);
}

static Object *first_b(const std::array<Object *, MAX_ARGS_NUM> &args, size_t args_num)
{
    Object *err = argNumCheck(args_num, 1);
    if (err)
        return err;

    if (args[0]->type_ != ARRAY)
    {
        char buffer[80];
        sprintf(buffer, "argument to 'first' must be ARRAY");
        return Object::alloc(buffer);
    }

    if (args_num > 0)
    {
        return args[0]->data.a.elems_[0];
    }

    return &Model::null_o;
}

static Object *last_b(const std::array<Object *, MAX_ARGS_NUM> &args, size_t args_num)
{
    Object *err = argNumCheck(args_num, 1);
    if (err)
        return err;

    if (args[0]->type_ != ARRAY)
    {
        char buffer[80];
        sprintf(buffer, "argument to 'last' must be ARRAY");
        return Object::alloc(buffer);
    }

    if (args_num > 0)
    {
        return args[0]->data.a.elems_[args[0]->data.a.num_ - 1];
    }

    return &Model::null_o;
}

static Object *rest_b(const std::array<Object *, MAX_ARGS_NUM> &args, size_t args_num)
{
    Object *err = argNumCheck(args_num, 1);
    if (err)
        return err;

    if (args[0]->type_ != ARRAY)
    {
        char buffer[80];
        sprintf(buffer, "argument to 'rest' must be ARRAY");
        return Object::alloc(buffer);
    }

    Object *arr = args[0];
    size_t length = arr->data.a.num_;
    if (length > 0)
    {
        return new Object(&arr->data.a.elems_[1], length - 1);
    }

    return &Model::null_o;
}

static Object *push_b(const std::array<Object *, MAX_ARGS_NUM> &args, size_t args_num)
{
    Object *err = argNumCheck(args_num, 2);
    if (err)
        return err;

    if (args[0]->type_ != ARRAY)
    {
        char buffer[80];
        sprintf(buffer, "argument to 'push' must be ARRAY");
        return Object::alloc(buffer);
    }

    Object **arr = args[0]->data.a.elems_;
    int elems_num = args[0]->data.a.num_;

    void *new_arr = realloc(arr, (elems_num + 1) * sizeof(Object *));
    args[0]->data.a.elems_ = static_cast<Object **>(new_arr);
    args[0]->data.a.num_++;
    args[0]->data.a.elems_[elems_num] = args[1];
    args[1]->retain();

    return args[0];
}

Builtins &Builtins::getInstance()
{
    static Builtins instance;
    return instance;
}

Object *Builtins::atIndex(size_t index) const
{
    const auto &iter = std::next(builtins_.cbegin(), index);
    Object *b = iter->second;
    b->retain();
    return b;
}

//TODO create tests for all builtins and check for memory leaks
Builtins::Builtins()
{
    builtins_.emplace("len", Object::alloc(len_b));
    builtins_.emplace("first", Object::alloc(first_b));
    builtins_.emplace("last", Object::alloc(last_b));
    builtins_.emplace("push", Object::alloc(push_b));
    builtins_.emplace("rest", Object::alloc(rest_b));
}

Object *Builtins::Get(const string &key)
{
    Builtins &instance = Builtins::getInstance();

    const auto &entry = instance.builtins_.find(key);

    if (entry != instance.builtins_.end())
    {
        entry->second->retain();
        return entry->second;
    }

    return nullptr;
}
