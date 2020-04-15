#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>

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

 struct Object
 {
     virtual ~Object() {}
     virtual ObjType type() const = 0;
     virtual std::string inspect() const = 0;
 };

struct Integer : public Object
{
    ObjType type() const { return INTEGER; }
    std::string inspect() const;

    int value_;
};


struct Boolean : public Object
{
    ObjType type() const { return BOOLEAN; }
    std::string inspect() const;

    bool value_;
};

struct Null : public Object
{
    ObjType type() const { return NUL; }
    std::string inspect() const { return "nullptr"; }
};




#endif
