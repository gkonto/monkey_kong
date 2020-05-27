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

struct Single;
class Bool;
class Null;
class Identifier;
class BlockStatement;
class Environment;

struct HashFn;
struct HashEqFn;
struct SinglePool;

using HashMap = std::unordered_map<Single *, Single *, HashFn, HashEqFn>;

extern std::unique_ptr<Pool<Single>> SingPool;

#define OBJECT_TYPES                                                           \
  X(INTEGER, "INTEGER")                                                        \
  X(BOOLEAN, "BOOLEAN")                                                        \
  X(NUL, "NULL")                                                               \
  X(RETURN, "RETURN")                                                          \
  X(ERROR, "ERROR")                                                            \
  X(FUNCTION, "FUNCTION")                                                      \
  X(STRING, "STRING")                                                          \
  X(BUILTIN, "BUILTIN")                                                        \
  X(ARRAY, "ARRAY")                                                            \
  X(HASH, "HASH")                                                              \
  X(COMPILED_FUNCTION, "COMPILED_FUNCTION")                                    \
  X(CLOSURE, "CLOSURE")                                                        \
  X(MAX, "MAX")

#define X(a, b) a,
enum ObjType { OBJECT_TYPES };
#undef X

#define X(a, b) b,
const char *const object_name[] = {OBJECT_TYPES};
#undef X

namespace Model {
extern Single true_o;
extern Single false_o;
extern Single null_o;
} // namespace Model

struct Single {
  /* CONSTRUCTORS */
  explicit Single(int value) : type_(INTEGER) { data.i.value_ = value; }
  explicit Single(bool value) : type_(BOOLEAN) { data.b.value_ = value; }
  explicit Single(Single *val) : type_(RETURN) { data.obj.obj_ = val; }
  explicit Single(char *msg) : type_(ERROR) { data.e.msg_ = strdup(msg); }
  explicit Single(HashMap *pairs) : type_(HASH) { data.hash.pairs_ = pairs; }
  explicit Single(Builtins::Function f) : type_(BUILTIN) { data.bltn.f_ = f; }
  explicit Single() : type_(NUL) {}
  explicit Single(const char *msg, ObjType type);
  explicit Single(Single **elements, int num);
  explicit Single(std::vector<Identifier *> *p, Environment *e,
                  BlockStatement *b);
  // ~CONSTRUCTORS

  void retain() { ++count_; }
  void release();
  template <typename... Args> static Single *alloc(Args... args);
  static void dealloc(Single *env);
  std::string inspect() const;
  std::string inspectArray() const;
  std::string inspectHash() const;
  std::string inspectString() const;
  std::string inspectFunction() const;
  std::string inspectInteger() const;
  std::string inspectError() const;
  std::string inspectBoolean() const;

  union {
    struct {
      int value_;
    } i; // Integer
    struct {
      bool value_;
    } b; // Boolean
    struct {
      Single *obj_;
    } obj; // Single
    struct {
      char *msg_;
    } e; // Error
    struct {
      std::vector<Identifier *> *params_;
      BlockStatement *body_;
      Environment *env_;
    } f; // function
    struct {
      char *value_;
    } s; // String
    struct {
      Builtins::Function f_;
    } bltn; // Builtin
    struct {
      Single **elems_;
      int num_;
    } a;
    struct {
      HashMap *pairs_;
    } hash; // Hash
  } data;
  ObjType type_;
  char count_ = 0;
};

template <typename... Args> Single *Single::alloc(Args... args) {
  if (!SingPool) {
    SingPool = std::make_unique<Pool<Single>>(50);
  }
  Single *newSingle = SingPool->alloc(std::forward<Args>(args)...);
  newSingle->retain();
  return newSingle;
}

struct HashFn {
  size_t operator()(const Single *p) const {
    std::stringstream ss;
    ss << p->type_ << p->inspect();
    std::hash<std::string> s_hash;
    size_t val = s_hash(ss.str());
    return val;
  }
};

struct HashEqFn {
  bool operator()(const Single *lhs, const Single *rhs) const {
    return lhs->type_ == rhs->type_ && lhs->inspect() == rhs->inspect();
  }
};

#endif
