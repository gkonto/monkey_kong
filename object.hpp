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
extern SinglePool *testPool;

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
    void retain() {
        ++count_;
        /*
        if (count_ == 1) {
            // auto molis ksekinise na xrisimopoieitai.
        }
        */
    }

    void release();

    Single *getNext() const { return next_; }
    void setNext(Single *next) {
        next_ = next;
    }

    union {
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
        Single *next_;
    } ;
    ObjType type_;
    char count_ = 0;
};

struct SinglePool {
    SinglePool() {
        firstAvailable_ = &objects_[0];
        for (int i = 0; i < POOL_SIZE - 1; ++i) {
            objects_[i].setNext(&objects_[i+1]);
        }
    }

    ~SinglePool() {
        std::cout << "Number of used objects: " << numOfAllocated_ << std::endl;
    }

    template<typename... Args>
    static Single *create(Args &&... args) {

        SinglePool &p = SinglePool::getInstance();
        assert(p.firstAvailable_ != nullptr);
        //Remove from the available list
        Single *newSingle = p.firstAvailable_;
        p.firstAvailable_ = newSingle->getNext();
        p.init(*newSingle, std::forward<Args>(args)...);
        p.numOfAllocated_++;
        newSingle->retain();
        return newSingle;
    }


    static SinglePool &getInstance() {
        return *testPool;
    }

    void makeAvailable(Single *obj) {
        obj->setNext(firstAvailable_);
        firstAvailable_ = obj;
        --numOfAllocated_;
    }

    void init(Single &obj, int value) {
        obj.type_ = INTEGER;
        obj.data.integer.value_ = value;
    }
    void init(Single &obj, bool value) {
        obj.type_ = BOOLEAN;
        obj.data.boolean.value_ = value;
    }
    void init(Single &obj, Single *val) {
        obj.type_ = RETURN;
        obj.data.obj.obj_ = val;
    }
    void init(Single &obj, char *msg) {
        obj.type_ = ERROR;
        obj.data.error.msg_ = msg;
    }
    void init(Single &obj, std::vector<Identifier *> *parameters, 
            Environment *env, 
            BlockStatement *body) {
        obj.type_ = FUNCTION;
        obj.data.function.parameters_ = parameters;
        obj.data.function.body_ = body;
        obj.data.function.env_ = env;
        env->retain();
    }

    private:
        int numOfAllocated_ = 0;
        static const int POOL_SIZE = 100;
        Single *firstAvailable_ = nullptr;
        Single objects_[POOL_SIZE];

};


#endif
