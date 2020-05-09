#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include "env.hpp"
#include "builtins.hpp"

struct Single;
class Bool;
class Null;
class Identifier;
class BlockStatement;
class Environment;

struct HashFunction;
struct HashEqualFunction;
struct SinglePool;

using HashMap = std::unordered_map<Single *, Single *, HashFunction, HashEqualFunction>;

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
     X(HASH, "HASH")\
     X(COMPILED_FUNCTION, "COMPILED_FUNCTION")\
     X(CLOSURE, "CLOSURE")\
     X(MAX, "MAX")

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
        data.error.msg_ = strdup(msg);
    }
    explicit Single(const char *msg, ObjType type) : type_(type) {
        data.string.value_ = strdup(msg);
    }
    explicit Single(Single **elements, int num) : type_(ARRAY) {
        data.array.elems_ = new Single *[num];
        memcpy(data.array.elems_, elements, sizeof(Single *) * num);
        data.array.num_ = num;
    }
    explicit Single(HashMap *pairs) 
        : type_(HASH) {
        data.hash.pairs_ = pairs;
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
   
    explicit Single(Builtins::Function f) : type_(BUILTIN) {
        data.builtin.f_ = f;
    }

    explicit Single() : type_(NUL) {}
    void retain() {  ++count_; }

    void release();

    template<typename... Args>
    static Single *alloc(Args... args);
    static void dealloc(Single *env);

    std::string inspect() const {
        if (type_ == INTEGER) {
            return inspectInteger();
        } else if (type_ == ARRAY) {
            return inspectArray();
        } else if (type_ == BOOLEAN) {
            return inspectBoolean();
        } else if (type_ == NUL) {
            return "null";
        } else if (type_ == RETURN) {
            return data.obj.obj_->inspect();
        } else if (type_ == ERROR) {
            return inspectError();
        } else if (type_ == FUNCTION) {
            return inspectFunction();
        } else if (type_ == STRING) {
            return  inspectString();
        } else if (type_ == BUILTIN) {
            return  "builtin function";
        } else if (type_ == ARRAY) {
            return  inspectArray();
        } else if (type_ == HASH) {
            return inspectHash();
        } else {
            return "ERROR: Needs inspect()";
        }
    }

    std::string inspectArray() const;
    std::string inspectHash() const;

    std::string inspectString() const {
        std::stringstream ss;
        ss << data.string.value_;
        return ss.str();
    }

    std::string inspectFunction() const;
        
    std::string inspectInteger() const {
        std::stringstream ss;
        ss << data.integer.value_;
        return ss.str();
    }

    std::string inspectError() const {
        std::stringstream ss;
        ss << "Error: ";
        ss << data.error.msg_;
        return ss.str();
    }

    std::string inspectBoolean() const {
        std::stringstream ss;
        ss << data.boolean.value_;
        return ss.str();
    }

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
        struct {
            char *value_;
        }string;
        struct {
            Builtins::Function f_;
        }builtin;
        struct {
            Single **elems_;
            int num_;
        } array;
        struct {   
            HashMap *pairs_;
        } hash;
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


struct HashFunction {
    size_t operator()(const Single *p) const {
        std::stringstream ss;
        ss << p->type_ << p->inspect();
        std::hash<std::string> s_hash;
        size_t val = s_hash(ss.str());
        return val;
    }
};

struct HashEqualFunction
{
    bool operator()(const Single *lhs, const Single *rhs) const
    {
        return lhs->type_ == rhs->type_ && lhs->inspect() == rhs->inspect();
    }
};

#endif
