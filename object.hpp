#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "builtins.hpp"
#include "env.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

struct Object;
class Bool;
class Null;
class Identifier;
class BlockStatement;
class Environment;
class FunctionImpl;

struct HashFn;
struct HashEqFn;
struct SinglePool;

using HashMap = std::unordered_map<Object *, Object *, HashFn, HashEqFn>;

extern std::unique_ptr<Pool<Object>> SingPool;

#define OBJECT_TYPES                        \
  X(INTEGER, "INTEGER")                     \
  X(BOOLEAN, "BOOLEAN")                     \
  X(NUL, "NULL")                            \
  X(RETURN, "RETURN")                       \
  X(ERROR, "ERROR")                         \
  X(FUNCTION, "FUNCTION")                   \
  X(STRING, "STRING")                       \
  X(BUILTIN, "BUILTIN")                     \
  X(ARRAY, "ARRAY")                         \
  X(HASH, "HASH")                           \
  X(COMPILED_FUNCTION, "COMPILED_FUNCTION") \
  X(CLOSURE, "CLOSURE")                     \
  X(MAX, "MAX")

#define X(a, b) a,
enum ObjType
{
  OBJECT_TYPES
};
#undef X

#define X(a, b) b,
const char *const object_name[] = {OBJECT_TYPES};
#undef X

namespace Model
{
  extern Object true_o;
  extern Object false_o;
  extern Object null_o;
} // namespace Model

struct Object
{
  /* CONSTRUCTORS */
  explicit Object(int value) : type_(INTEGER) { data.i.value_ = value; }
  explicit Object(bool value) : type_(BOOLEAN) { data.b.value_ = value; }
  explicit Object(Object *val) : type_(RETURN) { data.obj.obj_ = val; }
  explicit Object(char *msg) : type_(ERROR) { data.e.msg_ = strdup(msg); }
  explicit Object(HashMap *pairs) : type_(HASH) { data.hash.pairs_ = pairs; }
  explicit Object(Builtins::Function f) : type_(BUILTIN) { data.bltn.f_ = f; }
  Object() = default;
  explicit Object(const char *msg, ObjType type);
  explicit Object(Object **elements, int num);
  explicit Object(FunctionImpl *f, Environment *e);
  // ~CONSTRUCTORS

  void retain() { ++count_; }
  void release();
  template <typename... Args>
  static Object *alloc(Args... args);
  static void dealloc(Object *env);
  std::string inspect() const;
  std::string inspectArray() const;
  std::string inspectHash() const;
  std::string inspectString() const;
  std::string inspectFunction() const;
  std::string inspectInteger() const;
  std::string inspectError() const;
  std::string inspectBoolean() const;

  union
  {
    struct
    {
      int value_;
    } i; // Integer
    struct
    {
      bool value_;
    } b; // Boolean
    struct
    {
      Object *obj_;
    } obj; // Single
    struct
    {
      char *msg_;
    } e; // Error
    struct
    {
      FunctionImpl *func_;
      Environment *env_;
    } f; // function
    struct
    {
      char *value_;
    } s; // String
    struct
    {
      Builtins::Function f_;
    } bltn; // Builtin
    struct
    {
      Object **elems_;
      int num_;
    } a;
    struct
    {
      HashMap *pairs_;
    } hash; // Hash
  } data;
  ObjType type_ = NUL;
  char count_ = 0;
};

template <typename... Args>
Object *Object::alloc(Args... args)
{
  if (!SingPool)
  {
    SingPool = std::make_unique<Pool<Object>>(50);
  }
  Object *newSingle = SingPool->alloc(std::forward<Args>(args)...);
  newSingle->retain();
  return newSingle;
}

struct HashFn
{
  size_t operator()(const Object *p) const
  {
    std::stringstream ss;
    ss << p->type_ << p->inspect();
    std::hash<std::string> s_hash;
    size_t val = s_hash(ss.str());
    return val;
  }
};

struct HashEqFn
{
  bool operator()(const Object *lhs, const Object *rhs) const
  {
    return lhs->type_ == rhs->type_ && lhs->inspect() == rhs->inspect();
  }
};

#endif
