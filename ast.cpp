#include "ast.hpp"
#include "builtins.hpp"
#include "env.hpp"
#include <sstream>
#include <string.h>

using namespace std;

bool isTruthy(Single *obj) {
  if (obj == &Model::null_o) {
    return false;
  } else if (obj == &Model::true_o) {
    return true;
  } else if (obj == &Model::false_o) {
    return false;
  } else {
    return true;
  }
}

static bool isError(Single *val) {
  if (val) {
    return val->type_ == ERROR;
  }
  return false;
}

static Single *nativeBoolToSingObj(bool input) {
  return input ? &Model::true_o : &Model::false_o;
}

Let::~Let() {
  if (name_)
    delete name_;
  if (value_)
    delete value_;
}

PrefixExpression::~PrefixExpression() {
  if (right_)
    delete right_;
}

InfixExpression::~InfixExpression() {
  delete lhs_;
  delete rhs_;
}

string InfixExpression::asString() const {
  string ret;

  ret.append("(");
  ret.append(lhs_->asString());
  ret.append(" " + string(tok_names[op_]) + " ");
  if (rhs_) {
    ret.append(rhs_->asString());
  }
  ret.append(")");

  return ret;
}

string PrefixExpression::asString() const {
  string ret;

  ret.append("(");
  ret.append(operat_);
  if (right_) {
    ret.append(right_->asString());
  }
  ret.append(")");

  return ret;
}

Return::~Return() {
  if (returnValue_)
    delete returnValue_;
}

string Let::asString() const {
  string ret;
  ret.append(tokenLiteral());
  ret.append(" ");
  ret.append(name_->asString());
  ret.append(" = ");

  if (value_) { // TODO remove condition in the future
    ret.append(value_->asString());
  }

  ret.append(";");

  return ret;
}

string IntegerLiteral::asString() const {
  stringstream ss;
  ss << value_;
  return ss.str();
}

string Return::asString() const {
  string ret;
  ret.append(tokenLiteral() + " ");

  if (returnValue_) { // TODO remove condition in the future
    ret.append(returnValue_->asString());
  }

  ret.append(";");

  return ret;
}

string Program::asString() const {
  string ret;

  for (auto &a : statements_) {
    ret.append(a->asString());
  }

  return ret;
}

BlockStatement::~BlockStatement() {
  for (auto &stmt : statements_) {
    delete stmt;
  }
}

FunctionLiteral::~FunctionLiteral() {
  for (auto &param : parameters_) {
    delete param;
  }
  delete body_;
}

string If::asString() const {
  string ret;
  ret.append("if");
  ret.append(condition_->asString());
  ret.append(" ");
  ret.append(consequence_->asString());

  if (alternative_) {
    ret.append("else ");
    ret.append(alternative_->asString());
  }

  return ret;
}

string FunctionLiteral::asString() const {
  string ret;
  ret.append(tokenLiteral());
  ret.append("(");

  size_t i = 1;
  for (auto &a : parameters_) {
    ret.append(a->asString());
    if (i != parameters_.size()) {
      ret.append(", ");
    }
    i++;
  }
  ret.append(") ");
  ret.append(body_->asString());

  return ret;
}

string BlockStatement::asString() const {
  string ret;

  for (auto &a : statements_) {
    ret.append(a->asString());
  }

  return ret;
}

CallExpression::~CallExpression() {
  for (auto &arg : arguments_) {
    delete arg;
  }
  delete function_;
}

string CallExpression::asString() const {
  string ret;
  vector<string> args;

  ret.append(function_->asString());
  ret.append("(");

  size_t i = 1;
  for (auto &a : arguments_) {
    ret.append(a->asString());
    if (i != arguments_.size()) {
      ret.append(", ");
    }
    i++;
  }
  ret.append(")");

  return ret;
}


Single *Program::eval(Environment *s) {
  Single *ret = nullptr;
  for (auto &stmt : statements()) {
    if (ret)
      ret->release();

    ret = stmt->eval(s);
    if (ret && ret->type_ == RETURN) {
      Single *o = ret->data.obj.obj_;
      ret->release();

      return o;
    }

    if (ret && ret->type_ == ERROR) {
      return ret;
    }
  }
  return ret;
}

Single *Identifier::eval(Environment *s) {
  const string &key = value();
  Single *val = s->get(key);
  if (val) {
    val->retain();
    return val;
  }

  val = Builtins::Get(key);
  if (val) {
    val->retain();
    return val;
  }

  char buffer[80];
  sprintf(buffer, "identifier not found: %s", key.c_str());
  return Single::alloc(buffer);
}

Single *Let::eval(Environment *s) {
  Single *val = value()->eval(s);
  if (isError(val)) {
    return val;
  }
  s->set(name()->value(), val);
  return val;
}

Single *Return::eval(Environment *s) {
  Single *ret = value()->eval(s);
  if (isError(ret)) {
    return ret;
  }
  return Single::alloc(ret);
}

static Single *evalBangOperatorExpression(Single *right) {
  if (right == &Model::true_o) {
    return &Model::false_o;
  } else if (right == &Model::false_o) {
    return &Model::true_o;
  } else if (right == &Model::null_o) {
    return &Model::null_o;
  } else {
    return &Model::false_o;
  }
}

static Single *evalMinusPrefixOperatorExpression(Single *right) {
  if (right->type_ != INTEGER) {
    char buffer[80];
    sprintf(buffer, "unknown operator: -%s", object_name[right->type_]);
    return Single::alloc(buffer);
  }
  return Single::alloc(-right->data.i.value_);
}

static Single *evalPrefixExpression_core(const string &op, Single *right) {
  Single *ret = nullptr;
  if (!op.compare("!")) {
    ret = evalBangOperatorExpression(right);
  } else if (!op.compare("-")) {
    ret = evalMinusPrefixOperatorExpression(right);
  } else {
    char buffer[80];
    sprintf(buffer, "unknown operator: %s%s", op.c_str(),
            object_name[right->type_]);
    ret = Single::alloc(buffer);
  }
  if (right) {
    right->release();
  }
  return ret;
}

Single *PrefixExpression::eval(Environment *s) {
  Single *r = right()->eval(s);
  if (isError(r)) {
    return r;
  }
  return evalPrefixExpression_core(operator_s(), r);
}

static Single *evalStringInfixExpression(TokenType op, Single *left,
                                         Single *right) {
  if (op != T_PLUS) {
    char buffer[80];
    sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    return Single::alloc(buffer);
  }

  char buffer[1024];
  const char *l = left->data.s.value_;
  const char *r = right->data.s.value_;
  strcpy(buffer, l);
  strcat(buffer, r);
  return Single::alloc(buffer, STRING);
}

static Single *evalIntegerInfixExpression(TokenType op, Single *left,
                                          Single *right) {
  // TODO remove all those compare with an array of function callbacks
  Single *t = nullptr;

  if (op == T_PLUS) {
    t = Single::alloc(left->data.i.value_ + right->data.i.value_);
  } else if (op == T_MINUS) {
    t = Single::alloc(left->data.i.value_ - right->data.i.value_);
  } else if (op == T_ASTERISK) {
    t = Single::alloc(left->data.i.value_ * right->data.i.value_);
  } else if (op == T_SLASH) {
    t = Single::alloc(left->data.i.value_ / right->data.i.value_);
  } else if (op == T_LT) {
    t = nativeBoolToSingObj(left->data.i.value_ < right->data.i.value_);
  } else if (op == T_GT) {
    t = nativeBoolToSingObj(left->data.i.value_ > right->data.i.value_);
  } else if (op == T_EQ) {
    t = nativeBoolToSingObj(left->data.i.value_ == right->data.i.value_);
  } else if (op == T_NOT_EQ) {
    t = nativeBoolToSingObj(left->data.i.value_ != right->data.i.value_);
  } else {
    char buffer[80];
    sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    t = Single::alloc(buffer);
  }

  return t;
}

static Single *evalInfixExpression_core(TokenType op, Single *left,
                                        Single *right) {
  Single *temp = nullptr;
  if (left->type_ == INTEGER && right->type_ == INTEGER) {
    temp = evalIntegerInfixExpression(op, left, right);

  } else if (op == T_EQ) {
    temp = nativeBoolToSingObj(left == right);
  } else if (op == T_NOT_EQ) {
    temp = nativeBoolToSingObj(left != right);
  } else if (left->type_ != right->type_) {
    char buffer[80];
    sprintf(buffer, "type mismatch: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    temp = Single::alloc(buffer);
  } else if (left->type_ == STRING) {
    temp = evalStringInfixExpression(op, left, right);
  } else {
    char buffer[80];
    sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_],
            tok_names[op], object_name[right->type_]);
    temp = Single::alloc(buffer);
  }

  if (temp) {
    left->release();
    right->release();
  }
  return temp;
}

Single *InfixExpression::eval(Environment *s) {
  Single *l = lhs()->eval(s);
  if (isError(l)) {
    return l;
  }
  Single *r = rhs()->eval(s);
  if (isError(r)) {
    return r;
  }
  Single *ret = evalInfixExpression_core(op(), l, r);

  return ret;
}

Single *Boolean::eval(Environment *s) { return nativeBoolToSingObj(value()); }

Single *BlockStatement::eval(Environment *store) {
  Single *temp = nullptr;
  for (auto &s : statements()) {
    if (temp)
      temp->release();
    temp = s->eval(store);
    if (temp && (temp->type_ == RETURN || temp->type_ == ERROR)) {
      return temp;
    }
  }
  return temp;
}

Single *If::eval(Environment *s) {
  Single *cond = condition()->eval(s);
  if (isError(cond)) {
    return cond;
  }

  Single *ret = nullptr;
  if (isTruthy(cond)) {
    ret = consequence()->eval(s);
  } else if (alternative()) {
    ret = alternative()->eval(s);
  } else {
    ret = &Model::null_o;
  }

  cond->release();
  return ret;
}

Single *FunctionLiteral::eval(Environment *s) {
  vector<Identifier *> &params = parameters();
  BlockStatement *b = body();
  return Single::alloc(&params, s, b);
}

static bool evalExpressions(const vector<Node *> &args,
                            array<Single *, MAX_ARGS_NUM> &arg_to_return,
                            Environment *s) {

  size_t numArgs = args.size();

  if (numArgs > MAX_ARGS_NUM) {
    char buffer[80];
    sprintf(buffer, "Cannot support more than 25 args in a function");
    Single *err = Single::alloc(buffer);
    arg_to_return[0] = err;
    return true;
  }

  for (size_t i = 0; i < numArgs; i++) {
    Single *evaluated = args[i]->eval(s);
    if (isError(evaluated)) {
      // FIXME! mem leak here! Needs release osa mpikan sto arg_to_return prin
      // ftaso edo.
      arg_to_return[0] = evaluated;
      return true;
    }
    arg_to_return[i] = evaluated;
  }

  return false;
}

Environment *extendFunctionEnv(Single *fn,
                               array<Single *, MAX_ARGS_NUM> &args) {
  Environment *env =
      Environment::alloc(fn->data.f.env_); // FIXME deallocate?
  vector<Identifier *> &params = *(fn->data.f.params_);
  for (size_t i = 0; i < params.size(); i++) {
    Identifier *iden = params[i];
    env->set(iden->value(), args[i]);
  }

  return env;
}

// OK
static Single *unwrapReturnValue(Single *val) {
  if (val->type_ != RETURN) {
    return val;
  }
  Single *result = val->data.obj.obj_;
  val->release();
  return result;
}

static Single *applyFunction(Single *fn, array<Single *, MAX_ARGS_NUM> &args,
                             size_t args_num) {
  if (fn->type_ == BUILTIN) {
    return fn->data.bltn.f_(args, args_num);
  }

  if (fn->type_ == FUNCTION) {
    Environment *extended_env = extendFunctionEnv(fn, args);
    Single *ret = fn->data.f.body_->eval(extended_env);

    ret = unwrapReturnValue(ret);
    extended_env->release();
    return ret;
  }

  char buffer[80];
  sprintf(buffer, "not a function: %s\n", object_name[fn->type_]);
  return Single::alloc(buffer);
}

Single *CallExpression::eval(Environment *s) {
  Single *fn = function()->eval(s);
  if (isError(fn)) {
    return fn;
  }
  array<Single *, MAX_ARGS_NUM> applied_args;
  const vector<Node *> &args = arguments();
  bool isError = evalExpressions(args, applied_args, s);
  if (isError) {
    return applied_args[0];
  }
  // delete fn
  Single *ret = applyFunction(fn, applied_args, args.size());
  for (size_t i = 0; i < size(); i++) {
    applied_args[i]->release();
  }
  fn->release();
  return ret;
}

string StringLiteral::asString() const { return tok_->literal(); }

string ArrayLiteral::asString() const {
  string ret;

  ret.append("[");

  // TODO refactoring. I need the index during the loop
  // I should not add comma in the last argument.
  size_t i = 1;
  size_t total = elements_.size();
  for (auto &a : elements_) {
    ret.append(a->asString());
    if (i != total) {
      ret.append(", ");
    }
    i++;
  }
  ret.append("]");

  return ret;
}

ArrayLiteral::~ArrayLiteral() {
  for (auto &a : elements_) {
    delete a;
  }
}

string IndexExpression::asString() const {
  string ret;

  ret.append("(");
  ret.append(left_->asString());
  ret.append("[");
  ret.append(index_->asString());
  ret.append("])");

  return ret;
}

Single *ArrayLiteral::eval(Environment *s) {
  array<Single *, MAX_ARGS_NUM>
      eval_elems_; // TODO fix me. This must be a vector;
  const vector<Node *> &elems = elements();
  bool isError = evalExpressions(elems, eval_elems_, s);
  if (isError) {
    return eval_elems_[0];
  }
  return Single::alloc(&eval_elems_[0], elems.size());
}

static Single *evalArrayIndexExpression(Single *left, Single *index) {
  int idx = index->data.i.value_;
  int max = left->data.a.num_ - 1;
  if (idx < 0 || idx > max) {
    return &Model::null_o;
  }
  Single *ret = left->data.a.elems_[idx];
  ret->retain();
  return ret;
}

static Single *evalHashIndexExpression(Single *left, Single *index) {
  ObjType type = index->type_;

  if (type != INTEGER && type != STRING && type != BOOLEAN) {
    char buffer[80];
    sprintf(buffer, "unusable as hash key: %s", object_name[index->type_]);
    Single *ret = Single::alloc(buffer);

    left->release();
    index->release();
    return ret;
  }

  const HashMap &map = *left->data.hash.pairs_;
  const auto &entry = map.find(index);
  if (entry == map.end()) {
    return &Model::null_o;
  } else {
    entry->second->retain();
  }

  return entry->second;
}

static Single *evalIndexExpression(Single *left, Single *index) {
  if (left->type_ == ARRAY && index->type_ == INTEGER) {
    return evalArrayIndexExpression(left, index);
  } else if (left->type_ == HASH) {
    return evalHashIndexExpression(left, index);
  } else {
    char buffer[80];
    sprintf(buffer, "index operator not supported: %s",
            object_name[left->type_]);
    return Single::alloc(buffer);
  }
}

Single *IndexExpression::eval(Environment *s) {
  Single *l = left()->eval(s);
  if (l->type_ == ERROR) {
    return l;
  }
  Single *i = index()->eval(s);
  if (i->type_ == ERROR) {
    return i;
  }

  Single *ret = evalIndexExpression(l, i);
  l->release();
  i->release();

  return ret;
}

string HashLiteral::asString() const {
  string ret;
  ret.append("{");
  size_t i = 1;
  size_t total = pairs_.size();

  for (auto &a : pairs_) {
    ret.append(a.first->asString() + ":" + a.second->asString());
    if (i != total) {
      ret.append(", ");
    }
    i++;
  }
  ret.append("}");

  return ret;
}

static Single *evalHashLiteral(const HashLiteral *node, Environment *env) {
  HashMap *pairs = new HashMap;

  Single *hash = Single::alloc(pairs);

  for (auto &a : *node) {
    Single *key = a.first->eval(env);
    if (key && key->type_ == ERROR) {
      return key;
    }

    Single *value = a.second->eval(env);
    if (value && value->type_ == ERROR) {
      return value;
    }
    pairs->emplace(key, value);
  }

  return hash;
}

Single *HashLiteral::eval(Environment *env) {
  return evalHashLiteral(this, env);
}

If::~If() {
  if (consequence_)
    delete consequence_;
  if (alternative_)
    delete alternative_;
  if (condition_)
    delete condition_;
}

IndexExpression::~IndexExpression() {
    delete left_;
    if (index_) {
      delete index_;
    }
}

HashLiteral::~HashLiteral() {
    for (auto &a : pairs_) {
      delete a.first;
      delete a.second;
    }
}

Program::~Program() {
    for (auto &stmt : statements_) {
      delete stmt;
    }
    TokenPool.reset();
}

ExpressionStatement::~ExpressionStatement() {
    if (expression_)
      delete expression_;
}
