#include "ast.hpp"
#include "builtins.hpp"
#include "env.hpp"
#include <sstream>
#include <string.h>

using namespace std;

bool isTruthy(Object *obj)
{
  if (obj == &Model::null_o)
  {
    return false;
  }
  else if (obj == &Model::true_o)
  {
    return true;
  }
  else if (obj == &Model::false_o)
  {
    return false;
  }
  else
  {
    return true;
  }
}

static bool isError(Object *val)
{
  if (val)
  {
    return val->type_ == ERROR;
  }
  return false;
}

static Object *nativeBoolToSingObj(bool input)
{
  return input ? &Model::true_o : &Model::false_o;
}

Let::~Let()
{
  if (name_)
    delete name_;
  if (value_)
    delete value_;
}

PrefixExpression::~PrefixExpression()
{
  if (right_)
    delete right_;
}

InfixExpression::~InfixExpression()
{
  delete lhs_;
  delete rhs_;
}

string InfixExpression::asString() const
{
  string ret;

  ret.append("(");
  ret.append(lhs_->asString());
  ret.append(" " + string(tok_names[op_]) + " ");
  if (rhs_)
  {
    ret.append(rhs_->asString());
  }
  ret.append(")");

  return ret;
}

string PrefixExpression::asString() const
{
  string ret;

  ret.append("(");
  ret.append(operat_);
  if (right_)
  {
    ret.append(right_->asString());
  }
  ret.append(")");

  return ret;
}

Return::~Return()
{
  if (returnValue_)
    delete returnValue_;
}

string Let::asString() const
{
  string ret;
  ret.append(tokenLiteral());
  ret.append(" ");
  ret.append(name_->asString());
  ret.append(" = ");

  if (value_)
  { // TODO remove condition in the future
    ret.append(value_->asString());
  }

  ret.append(";");

  return ret;
}

string IntegerLiteral::asString() const
{
  stringstream ss;
  ss << value_;
  return ss.str();
}

string Return::asString() const
{
  string ret;
  ret.append(tokenLiteral() + " ");

  if (returnValue_)
  { // TODO remove condition in the future
    ret.append(returnValue_->asString());
  }

  ret.append(";");

  return ret;
}

string Program::asString() const
{
  string ret;

  for (auto &a : statements_)
  {
    ret.append(a->asString());
  }

  return ret;
}

BlockStatement::~BlockStatement()
{
  for (auto &stmt : statements_)
  {
    delete stmt;
  }
}

FunctionImpl::~FunctionImpl()
{
  for (auto &param : parameters_)
  {
    delete param;
  }
  delete body_;
}

string If::asString() const
{
  string ret;
  ret.append("if");
  ret.append(condition_->asString());
  ret.append(" ");
  ret.append(consequence_->asString());

  if (alternative_)
  {
    ret.append("else ");
    ret.append(alternative_->asString());
  }

  return ret;
}

string FunctionLiteral::asString() const
{
  string ret;
  ret.append(tokenLiteral());
  ret.append("(");

  size_t i = 1;
  for (auto &a : f_->parameters())
  {
    ret.append(a->asString());
    if (i != f_->parameters().size())
    {
      ret.append(", ");
    }
    i++;
  }
  ret.append(") ");
  ret.append(f_->body()->asString());

  return ret;
}

string BlockStatement::asString() const
{
  string ret;

  for (auto &a : statements_)
  {
    ret.append(a->asString());
  }

  return ret;
}

CallExpression::~CallExpression()
{
  for (auto &arg : arguments_)
  {
    delete arg;
  }
  delete function_;
}

string CallExpression::asString() const
{
  string ret;
  vector<string> args;

  ret.append(function_->asString());
  ret.append("(");

  size_t i = 1;
  for (auto &a : arguments_)
  {
    ret.append(a->asString());
    if (i != arguments_.size())
    {
      ret.append(", ");
    }
    i++;
  }
  ret.append(")");

  return ret;
}

Object *Program::eval(Environment *s)
{
  Object *ret = nullptr;
  for (auto &stmt : statements())
  {
    if (ret)
      ret->release();

    ret = stmt->eval(s);
    if (ret && ret->type_ == RETURN)
    {
      Object *o = ret->data.obj.obj_;
      ret->release();

      return o;
    }

    if (ret && ret->type_ == ERROR)
    {
      return ret;
    }
  }
  return ret;
}

Object *Identifier::eval(Environment *s)
{
  Object *val = s->get(this);
  if (val)
  {
    val->retain();
    return val;
  }
  
  const string &key = value();

  val = Builtins::Get(key);
  if (val)
  {
    val->retain();
    return val;
  }

  char buffer[80];
  sprintf_s(buffer, "identifier not found: %s", key.c_str());
  return Object::alloc(buffer);
}

Object *Let::eval(Environment *s)
{
  Object *val = value()->eval(s);
  if (isError(val))
  {
    return val;
  }
  s->set(name()->value(), val);
  return val;
}

Object *Return::eval(Environment *s)
{
  Object *ret = value()->eval(s);
  if (isError(ret))
  {
    return ret;
  }
  return Object::alloc(ret);
}

static Object *evalBangOperatorExpression(Object *right)
{
  if (right == &Model::true_o)
  {
    return &Model::false_o;
  }
  else if (right == &Model::false_o)
  {
    return &Model::true_o;
  }
  else if (right == &Model::null_o)
  {
    return &Model::null_o;
  }
  else
  {
    return &Model::false_o;
  }
}

static Object *evalMinusPrefixOperatorExpression(Object *right)
{
  if (right->type_ != INTEGER)
  {
    char buffer[80];
    sprintf_s(buffer, "unknown operator: -%s", object_name[right->type_]);
    return Object::alloc(buffer);
  }
  return Object::alloc(-right->data.i.value_);
}

static Object *evalPrefixExpression_core(const string &op, Object *right)
{
  Object *ret = nullptr;
  if (!op.compare("!"))
  {
    ret = evalBangOperatorExpression(right);
  }
  else if (!op.compare("-"))
  {
    ret = evalMinusPrefixOperatorExpression(right);
  }
  else
  {
    char buffer[80];
    sprintf_s(buffer, "unknown operator: %s%s", op.c_str(),
            object_name[right->type_]);
    ret = Object::alloc(buffer);
  }
  if (right)
  {
    right->release();
  }
  return ret;
}

Object *PrefixExpression::eval(Environment *s)
{
  Object *r = right()->eval(s);
  if (isError(r))
  {
    return r;
  }
  return evalPrefixExpression_core(operator_s(), r);
}

static Object *evalStringInfixExpression(TokenType op, Object *left,
                                         Object *right)
{
  if (op != T_PLUS)
  {
    char buffer[80];
    sprintf_s(buffer, "unknown operator: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    return Object::alloc(buffer);
  }

  char buffer[1024];
  const char *l = left->data.s.value_;
  const char *r = right->data.s.value_;
  strcpy_s(buffer, l);
  strcat_s(buffer, r);
  return Object::alloc(buffer, STRING);
}

static Object *evalIntegerInfixExpression(TokenType op, Object *left,
                                          Object *right)
{
  // TODO remove all those compare with an array of function callbacks
  Object *t = nullptr;

  if (op == T_PLUS)
  {
    t = Object::alloc(left->data.i.value_ + right->data.i.value_);
  }
  else if (op == T_MINUS)
  {
    t = Object::alloc(left->data.i.value_ - right->data.i.value_);
  }
  else if (op == T_ASTERISK)
  {
    t = Object::alloc(left->data.i.value_ * right->data.i.value_);
  }
  else if (op == T_SLASH)
  {
    t = Object::alloc(left->data.i.value_ / right->data.i.value_);
  }
  else if (op == T_LT)
  {
    t = nativeBoolToSingObj(left->data.i.value_ < right->data.i.value_);
  }
  else if (op == T_GT)
  {
    t = nativeBoolToSingObj(left->data.i.value_ > right->data.i.value_);
  }
  else if (op == T_EQ)
  {
    t = nativeBoolToSingObj(left->data.i.value_ == right->data.i.value_);
  }
  else if (op == T_NOT_EQ)
  {
    t = nativeBoolToSingObj(left->data.i.value_ != right->data.i.value_);
  }
  else
  {
    char buffer[80];
    sprintf_s(buffer, "unknown operator: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    t = Object::alloc(buffer);
  }

  return t;
}

static Object *evalInfixExpression_core(TokenType op, Object *left,
                                        Object *right)
{
  Object *temp = nullptr;
  if (left->type_ == INTEGER && right->type_ == INTEGER)
  {
    temp = evalIntegerInfixExpression(op, left, right);
  }
  else if (op == T_EQ)
  {
    temp = nativeBoolToSingObj(left == right);
  }
  else if (op == T_NOT_EQ)
  {
    temp = nativeBoolToSingObj(left != right);
  }
  else if (left->type_ != right->type_)
  {
    char buffer[80];
    sprintf_s(buffer, "type mismatch: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    temp = Object::alloc(buffer);
  }
  else if (left->type_ == STRING)
  {
    temp = evalStringInfixExpression(op, left, right);
  }
  else
  {
    char buffer[80];
    sprintf_s(buffer, "unknown operator: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    temp = Object::alloc(buffer);
  }

  return temp;
}

Object *InfixExpression::eval(Environment *s)
{
  Object *l = lhs()->eval(s);

  if (isError(l))
  {
    return l;
  }

  Object *r = rhs()->eval(s);
  if (isError(r))
  {
    return r;
  }

  Object *ret = evalInfixExpression_core(op(), l, r);

  l->release();
  r->release();

  return ret;
}

Object *Boolean::eval(Environment *s) { return nativeBoolToSingObj(value()); }

Object *BlockStatement::eval(Environment *store)
{
  Object *temp = nullptr;
  for (auto &s : statements())
  {
    if (temp)
      temp->release();
    temp = s->eval(store);
    if (temp && (temp->type_ == RETURN || temp->type_ == ERROR))
    {
      return temp;
    }
  }
  return temp;
}

Object *If::eval(Environment *s)
{
  Object *cond = condition()->eval(s);
  if (isError(cond))
  {
    return cond;
  }

  Object *ret = nullptr;
  if (isTruthy(cond))
  {
    ret = consequence()->eval(s);
  }
  else if (alternative())
  {
    ret = alternative()->eval(s);
  }
  else
  {
    ret = &Model::null_o;
  }

  cond->release();
  return ret;
}

Object *FunctionLiteral::eval(Environment *s)
{
  FunctionImpl *i = impl();
  return Object::alloc(i, s);
}

static bool evalExpressions(const vector<Node *> &args,
                            array<Object *, MAX_ARGS_NUM> &arg_to_return,
                            Environment *s)
{

  size_t numArgs = args.size();

  if (numArgs > MAX_ARGS_NUM)
  {
    char buffer[80];
    sprintf_s(buffer, "Cannot support more than 25 args in a function");
    Object *err = Object::alloc(buffer);
    arg_to_return[0] = err;
    return true;
  }

  for (size_t i = 0; i < numArgs; i++)
  {
    Object *evaluated = args[i]->eval(s);
    if (isError(evaluated))
    {
      // FIXME! mem leak here! Needs release osa mpikan sto arg_to_return prin
      // ftaso edo.
      arg_to_return[0] = evaluated;
      return true;
    }
    arg_to_return[i] = evaluated;
  }

  return false;
}

FunctionEnvironment *extendFunctionEnv(Object *fn,
                               array<Object *, MAX_ARGS_NUM> &args)
{
  const vector<Identifier *> &params = fn->data.f.func_->parameters();
  FunctionEnvironment *env = new FunctionEnvironment(fn->data.f.env_, params.size(), args);
  return env;
}

// OK
static Object *unwrapReturnValue(Object *val)
{
  if (val->type_ != RETURN)
  {
    return val;
  }
  Object *result = val->data.obj.obj_;
  val->release();
  return result;
}

static Object *applyFunction(Object *fn, array<Object *, MAX_ARGS_NUM> &args,
                             size_t args_num)
{
  if (fn->type_ == BUILTIN)
  {
    return fn->data.bltn.f_(args, args_num);
  }

  if (fn->type_ == FUNCTION)
  {
    Environment *extended_env = extendFunctionEnv(fn, args);
    Object *ret = fn->data.f.func_->body()->eval(extended_env);

    ret = unwrapReturnValue(ret);
    extended_env->release();
    return ret;
  }

  char buffer[80];
  sprintf_s(buffer, "not a function: %s\n", object_name[fn->type_]);
  return Object::alloc(buffer);
}

Object *CallExpression::eval(Environment *s)
{
  Object *fn = function()->eval(s);
  if (isError(fn))
  {
    return fn;
  }
  array<Object *, MAX_ARGS_NUM> applied_args;
  const vector<Node *> &args = arguments();
  bool isError = evalExpressions(args, applied_args, s);
  if (isError)
  {
    return applied_args[0];
  }
  // delete fn
  Object *ret = applyFunction(fn, applied_args, args.size());
  for (size_t i = 0; i < size(); i++)
  {
    applied_args[i]->release();
  }
  fn->release();
  return ret;
}

string StringLiteral::asString() const { return tok_->literal(); }

string ArrayLiteral::asString() const
{
  string ret;

  ret.append("[");

  // TODO refactoring. I need the index during the loop
  // I should not add comma in the last argument.
  size_t i = 1;
  size_t total = elements_.size();
  for (auto &a : elements_)
  {
    ret.append(a->asString());
    if (i != total)
    {
      ret.append(", ");
    }
    i++;
  }
  ret.append("]");

  return ret;
}

ArrayLiteral::~ArrayLiteral()
{
  for (auto &a : elements_)
  {
    delete a;
  }
}

string IndexExpression::asString() const
{
  string ret;

  ret.append("(");
  ret.append(left_->asString());
  ret.append("[");
  ret.append(index_->asString());
  ret.append("])");

  return ret;
}

Object *ArrayLiteral::eval(Environment *s)
{
  array<Object *, MAX_ARGS_NUM>
      eval_elems_; // TODO fix me. This must be a vector;
  const vector<Node *> &elems = elements();
  bool isError = evalExpressions(elems, eval_elems_, s);
  if (isError)
  {
    return eval_elems_[0];
  }
  return Object::alloc(&eval_elems_[0], elems.size());
}

static Object *evalArrayIndexExpression(Object *left, Object *index)
{
  int idx = index->data.i.value_;
  int max = left->data.a.num_ - 1;
  if (idx < 0 || idx > max)
  {
    return &Model::null_o;
  }
  Object *ret = left->data.a.elems_[idx];
  ret->retain();
  return ret;
}

static Object *evalHashIndexExpression(Object *left, Object *index)
{
  ObjType type = index->type_;

  if (type != INTEGER && type != STRING && type != BOOLEAN)
  {
    char buffer[80];
    sprintf_s(buffer, "unusable as hash key: %s", object_name[index->type_]);
    Object *ret = Object::alloc(buffer);

    left->release();
    index->release();
    return ret;
  }

  const HashMap &map = *left->data.hash.pairs_;
  const auto &entry = map.find(index);
  if (entry == map.end())
  {
    return &Model::null_o;
  }
  else
  {
    entry->second->retain();
  }

  return entry->second;
}

static Object *evalIndexExpression(Object *left, Object *index)
{
  if (left->type_ == ARRAY && index->type_ == INTEGER)
  {
    return evalArrayIndexExpression(left, index);
  }
  else if (left->type_ == HASH)
  {
    return evalHashIndexExpression(left, index);
  }
  else
  {
    char buffer[80];
    sprintf_s(buffer, "index operator not supported: %s",
            object_name[left->type_]);
    return Object::alloc(buffer);
  }
}

Object *IndexExpression::eval(Environment *s)
{
  Object *l = left()->eval(s);
  if (l->type_ == ERROR)
  {
    return l;
  }
  Object *i = index()->eval(s);
  if (i->type_ == ERROR)
  {
    return i;
  }

  Object *ret = evalIndexExpression(l, i);
  l->release();
  i->release();

  return ret;
}

string HashLiteral::asString() const
{
  string ret;
  ret.append("{");
  size_t i = 1;
  size_t total = pairs_.size();

  for (auto &a : pairs_)
  {
    ret.append(a.first->asString() + ":" + a.second->asString());
    if (i != total)
    {
      ret.append(", ");
    }
    i++;
  }
  ret.append("}");

  return ret;
}

static Object *evalHashLiteral(const HashLiteral *node, Environment *env)
{
  HashMap *pairs = new HashMap;

  Object *hash = Object::alloc(pairs);

  for (auto &a : *node)
  {
    Object *key = a.first->eval(env);
    if (key && key->type_ == ERROR)
    {
      return key;
    }

    Object *value = a.second->eval(env);
    if (value && value->type_ == ERROR)
    {
      return value;
    }
    pairs->emplace(key, value);
  }

  return hash;
}

Object *HashLiteral::eval(Environment *env)
{
  return evalHashLiteral(this, env);
}

If::~If()
{
  if (consequence_)
    delete consequence_;
  if (alternative_)
    delete alternative_;
  if (condition_)
    delete condition_;
}

IndexExpression::~IndexExpression()
{
  delete left_;
  if (index_)
  {
    delete index_;
  }
}

HashLiteral::~HashLiteral()
{
  for (auto &a : pairs_)
  {
    delete a.first;
    delete a.second;
  }
}

Program::~Program()
{
  for (auto &stmt : statements_)
  {
    delete stmt;
  }
  TokenPool.reset();
}

ExpressionStatement::~ExpressionStatement()
{
  if (expression_)
    delete expression_;
}


void Program::display(int depth) const
{
  std::cout << std::endl;
  std::cout << "-Program(array of Nodes)" << std::endl;
  ++depth;
  for (auto p : statements_) {
    p->display(depth);
  }
}

void Identifier::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-Identifier(string)" << std::endl;
  std::cout << indent << "Value: " << value_ << std::endl;
  std::cout << indent << "VariableNumber: " << index_ << std::endl;
}

void Let::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-Let(name, value)" << std::endl;
  name_->display(++depth);
  value_->display(depth);
}

void Return::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-Return(Node)" << std::endl;
  returnValue_->display(++depth);
}

void ExpressionStatement::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-ExpressionStatement(Expression)" << std::endl;
  expression_->display(++depth);
}

void IntegerLiteral::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-IntegerLiteral(integer)" << std::endl;
  std::cout << indent << value_ << std::endl;
}

void PrefixExpression::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-PrefixExpression(operat, Right)" << std::endl;
  std::cout << indent << operat_ << std::endl;
  right_->display(++depth);
}

void InfixExpression::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-InfixExpression(lhs, rhs)" << std::endl;
  ++depth;
  lhs_->display(depth);
  rhs_->display(depth);
}

void Boolean::display(int depth) const
{
  string indent(depth, '\t');
    std::cout << indent << "-Boolean(value)" << std::endl;
    std::cout << depth << value_ << std::endl;
}

void BlockStatement::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-BlockStatement(Statement array)" << std::endl;
  ++depth;
  for (auto a : statements_) {
    a->display(depth);
  }
}

void If::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-If(Condition, Consequent, Alternative)" << std::endl;
  ++depth;
  condition_->display(depth);
  consequence_->display(depth);
  alternative_->display(depth);
}

void FunctionLiteral::display(int depth) const
{
  string indent(depth, '\t');
  size_t numOfParams = f_->parameters().size();
  std::cout << indent << "-FunctionLiteral(Parameters[" << numOfParams << "], Body)" << std::endl;
  ++depth;
  for (auto a : f_->parameters()) {
    a->display(depth);
  }
  f_->body()->display(depth);
}

void CallExpression::display(int depth) const
{
  string indent(depth, '\t');
  size_t numOfParams = arguments_.size();
  std::cout << indent << "-CallExpression(Arguments[" << numOfParams << "], Function)" << std::endl;
  ++depth;
  for (auto a : arguments_) {
    a->display(depth);
  }
  function_->display(depth);
}

void StringLiteral::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-StringLiteral(value)" << std::endl;
  std::cout << indent << value_ << std::endl;
}

void ArrayLiteral::display(int depth) const
{
  string indent(depth, '\t');
  size_t numOfVals = elements_.size();
  std::cout << indent << "-ArrayLiteral[" << numOfVals << "]" << std::endl;
  ++depth;
  for (auto a : elements_) {
    a->display(depth);
  }
}

void IndexExpression::display(int depth) const
{
  string indent(depth, '\t');
  std::cout << indent << "-IndexExpression(Left, Right)" << std::endl;
  ++depth;
  left_->display(depth);
  index_->display(depth);
}

void HashLiteral::display(int depth) const
{
    string indent(depth, '\t');
    std::cout << indent << "HashLiteral" << std::endl;
    ++depth;
    for (auto p : pairs_) {
      p.first->display(depth);
      p.second->display(depth);
      std::cout << "======" << std::endl;
    }
}
