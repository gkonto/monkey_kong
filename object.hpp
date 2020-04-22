#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include "env.hpp"

struct Single;
class Bool;
class Null;
class Identifier;
class BlockStatement;
class Environment;

struct SinglePool;
extern std::unique_ptr<Pool<Single>> SingPool;

 #define OBJECT_TYPES\
     X(INTEGER, "INTEGER")\
     X(BOOLEAN, "BOOLEAN")\
     X(NUL,    "NULL")\
     X(RETURN, "RETURN")\
     X(ERROR, "ERROR")\
     X(FUNCTION, "FUNCTION")\
     X(STRING, "STRING")\
     X(BUILTIN, "BUILTIN")\
     X(ARRAY,   "ARRAY")\
     X(HASH_KEY, "HASH_KEY")\
     X(COMPILED_FUNCTION, "COMPILED_FUNCTION")\
     X(CLOSURE, "CLOSURE")\
     X(MAX, "MAX")\

 #define X(a, b) a,
 enum ObjType
 {
     OBJECT_TYPES
 };
 #undef X

 #define X(a, b) b,
 const char *const object_name[] = {
     OBJECT_TYPES
 };
 #undef X


namespace Model {
    extern Single true_o;
    extern Single false_o;
    extern Single null_o;
}

struct Single
{
    explicit Single(int value) : type_(INTEGER) {
        data.integer.value_ = value;
    }
    explicit Single(bool value) : type_(BOOLEAN) {
        data.boolean.value_ = value;
    }
    explicit Single(Single *val) : type_(RETURN) {
        data.obj.obj_ = val;
    }
    explicit Single(char *msg) : type_(ERROR) {
        data.error.msg_ = msg;
    }
    explicit Single(std::vector<Identifier *> *parameters, 
            Environment *env, 
            BlockStatement *body)
    : type_(FUNCTION) 
    {
        data.function.parameters_ = parameters;
        data.function.body_ = body;
        data.function.env_ = env;
        env->retain();
    }
    explicit Single() : type_(NUL) {
    }
    void retain() {  ++count_; }

    void release();

    template<typename... Args>
    static Single *alloc(Args... args);
    static void dealloc(Single *env);

    union {
        struct {
            int value_;
        } integer;
        struct {
            bool value_;
        } boolean;
        struct {
            Single *obj_;
        } obj;
        struct {
            char *msg_;
        } error;
        struct {
            std::vector<Identifier *> *parameters_;
            BlockStatement *body_; 
            Environment *env_;
        } function;
    } data;
    ObjType type_;
    char count_ = 0;
};

template<typename... Args>
Single  *Single::alloc(Args... args) {
    if (!SingPool) {
        SingPool = std::make_unique<Pool<Single>>(50);
    }
    Single *newSingle = SingPool->alloc(std::forward<Args>(args)...);
    newSingle->retain();
    return newSingle;
}


#endif
