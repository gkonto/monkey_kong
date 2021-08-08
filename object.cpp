#include "object.hpp"
#include "ast.hpp"
#include "env.hpp"
#include <sstream>
#include <string>

SinglePool *testPool = nullptr;
using namespace std;

unique_ptr<Pool<Object>> SingPool = nullptr;

namespace Model
{
  Object true_o(true);
  Object false_o(false);
  Object null_o;
} // namespace Model

void Object::release()
{
  --count_;
  if (count_ == 0)
  {
    if (this == &Model::false_o || this == &Model::true_o ||
        this == &Model::null_o)
    {
      return;
    }

    if (type_ == ERROR || type_ == STRING)
    {
      free(data.e.msg_);
    }
    else if (type_ == FUNCTION)
    {
      data.f.env_->release();
      data.f.func_->release();
    }
    else if (type_ == ARRAY)
    {
      if (data.a.num_)
      {
        delete[] data.a.elems_;
      }
    }
    else if (type_ == HASH)
    {
      if (data.hash.pairs_)
      {
        for (auto &a : *data.hash.pairs_)
        {
          a.first->release();
          a.second->release();
        }
        delete data.hash.pairs_;
      }
    }

    Object::dealloc(this);
  }
}

void Object::dealloc(Object *o)
{
  if (!o)
    return;
  assert(SingPool);
  if (SingPool)
  {
    SingPool->free(o);
  }
}

string Object::inspectFunction() const
{
  string ret;
  ret.append("fn");
  ret.append("(");

  size_t i = 1;
  size_t num = data.f.func_->parameters().size();

  for (auto &a : data.f.func_->parameters())
  {
    ret.append(a->asString());
    if (i != num)
    {
      i++;
      ret.append(", ");
    }
  }
  ret.append(") {\n");
  ret.append(data.f.func_->body()->asString());
  ret.append("\n}");

  return ret;
}

string Object::inspectArray() const
{
  string ret;
  ret.append("[");

  // TODO refactoring
  size_t i = 1;
  size_t num = data.a.num_;

  for (int k = 0; k < data.a.num_; ++k)
  {

    ret.append(data.a.elems_[k]->inspect());
    if (i != num)
    {
      i++;
      ret.append(", ");
    }
  }
  ret.append("]");

  return ret;
}

string Object::inspectHash() const
{
  string ret;
  size_t i = 1;
  size_t num = data.hash.pairs_->size();

  ret.append("{");
  for (auto &a : *data.hash.pairs_)
  {
    ret.append(a.first->inspect());
    ret.append(": ");
    ret.append(a.second->inspect());

    if (i != num)
    {
      i++;
      ret.append(", ");
    }
  }
  ret.append("}");

  return ret;
}

string Object::inspectBoolean() const
{
  stringstream ss;
  ss << data.b.value_;
  return ss.str();
}

string Object::inspectError() const
{
  stringstream ss;
  ss << "Error: ";
  ss << data.e.msg_;
  return ss.str();
}

string Object::inspectInteger() const
{
  stringstream ss;
  ss << data.i.value_;
  return ss.str();
}

string Object::inspectString() const
{
  stringstream ss;
  ss << data.s.value_;
  return ss.str();
}

string Object::inspect() const
{
  if (type_ == INTEGER)
  {
    return inspectInteger();
  }
  else if (type_ == ARRAY)
  {
    return inspectArray();
  }
  else if (type_ == BOOLEAN)
  {
    return inspectBoolean();
  }
  else if (type_ == NUL)
  {
    return "null";
  }
  else if (type_ == RETURN)
  {
    return data.obj.obj_->inspect();
  }
  else if (type_ == ERROR)
  {
    return inspectError();
  }
  else if (type_ == FUNCTION)
  {
    return inspectFunction();
  }
  else if (type_ == STRING)
  {
    return inspectString();
  }
  else if (type_ == BUILTIN)
  {
    return "builtin function";
  }
  else if (type_ == ARRAY)
  {
    return inspectArray();
  }
  else if (type_ == HASH)
  {
    return inspectHash();
  }
  else
  {
    return "ERROR: Needs inspect()";
  }
}

Object::Object(Object **elements, int num) : type_(ARRAY)
{
  data.a.elems_ = new Object *[num];
  memcpy(data.a.elems_, elements, sizeof(Object *) * num);
  data.a.num_ = num;
}

Object::Object(FunctionImpl *f, Environment *e) : type_(FUNCTION)
{
  data.f.func_ = f;
  data.f.env_ = e;
  f->retain();
  e->retain();
}

Object::Object(const char *msg, ObjType type) : type_(type)
{
  data.s.value_ = strdup(msg);
}
