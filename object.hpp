#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>

class Single;
class Bool;
class Null;

 #define OBJECT_TYPES\
     X(INTEGER, "INTEGER")\
     X(BOOLEAN, "BOOLEAN")\
     X(NUL,    "NULL")\
     X(RETURN_VALUE, "RETURN_VALUE")\
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

#include <iostream>
struct Single
{
    explicit Single(int value) : type_(INTEGER) {
        data.integer.value_ = value;
    }
    explicit Single(bool value) : type_(BOOLEAN) {
        data.boolean.value_ = value;
    }
    explicit Single() : type_(NUL) {}

    void operator delete(void *p) {
        if (p == &Model::false_o || p == &Model::true_o || p == &Model::null_o) {
            return;
        }
        free(p);
    }

    ObjType type_;
    union {
        struct {
            int value_;
        } integer;
        struct {
            bool value_;
        } boolean;
    }data;
};

 struct Object
 {
     //virtual ~Object() {}
     //void operator delete(void *p) {}
     virtual ObjType type() const = 0;
     virtual std::string inspect() const = 0;
 };

struct Integer : public Object
{
    explicit Integer(int val) : value_(val) {}
    ObjType type() const { return INTEGER; }
    std::string inspect() const;
    int value() const { return value_; }

    int value_;
};


#include <iostream>
struct Bool : public Object
{
    void operator delete(void *p) {
        std::cout << "Bool Overloading delete operator" << std::endl;
    }
    explicit Bool(bool val) : value_(val) {}
    ObjType type() const { return BOOLEAN; }
    std::string inspect() const;
    bool value() const { return value_; }

    bool value_;
};

struct Null : public Object
{
    void operator delete(void *p) {
        std::cout << "Overloading delete operator" << std::endl;
    }
    ObjType type() const { return NUL; }
    std::string inspect() const { return "nullptr"; }
};




#endif
