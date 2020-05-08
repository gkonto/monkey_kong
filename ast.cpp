#include <sstream>
#include <string.h>
#include "ast.hpp"
#include "env.hpp"
#include "visitor.hpp"
#include "builtins.hpp"

#ifdef NODISPATCH
Single *Node::eval(Environment *s) {
    Single *temp = nullptr;
    switch (type_) {
        case AST_PROGRAM: {
            Program *p = static_cast<Program *>(this);
            temp = p->evalProgram(s);
            break;
        }
        case AST_IDENTIFIER: {
            Identifier *p = static_cast<Identifier *>(this);
            temp = p->evalIdentifier(s);
            break;
        }
        case AST_LET: {
            Let *p = static_cast<Let *>(this);
            temp = p->evalLet(s);
            break;
        }
        case AST_RETURN: {
            Return *p = static_cast<Return *>(this);
            temp = p->evalReturn(s);
            break;
        }
        case AST_EXPRESSIONSTATEMENT: {
            ExpressionStatement *p = static_cast<ExpressionStatement *>(this);
            temp = p->evalExpressionStatement(s);
            break;
        }
        case AST_INTEGERLITERAL: {
            IntegerLiteral *p = static_cast<IntegerLiteral *>(this);
            temp = p->evalIntegerLiteral(s);
            break;
        }
        case AST_INFIXEXPRESSION: {
            InfixExpression *p = static_cast<InfixExpression *>(this);
            temp = p->evalInfixExpression(s);
            break;
        }
        case AST_PREFIXEXPRESSION: {
            PrefixExpression *p = static_cast<PrefixExpression *>(this);
            temp = p->evalPrefixExpression(s);
            break;
        }
        case AST_BOOLEAN: {
            Boolean *p = static_cast<Boolean *>(this);
            temp = p->evalBoolean(s);
            break;
        }
        case AST_BLOCKSTATEMENT: {
            BlockStatement *p = static_cast<BlockStatement *>(this);
            temp = p->evalBlockStatement(s);
            break;
        }
        case AST_IF: {
            If *p = static_cast<If *>(this);
            temp = p->evalIf(s);
            break;
        }
        case AST_FUNCTIONLITERAL: {
            FunctionLiteral *p = static_cast<FunctionLiteral *>(this);
            temp = p->evalFunctionLiteral(s);
            break;
        }
        case AST_CALLEXPRESSION: {
            CallExpression *p = static_cast<CallExpression *>(this);
            temp = p->evalCallExpression(s);
            break;
        }
       default:
            exit(1);
    }
    return temp;    
}
#endif


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

Let::~Let()
{
    if (name_) delete name_;
    if (value_) delete value_;
    //if ()  delete tok_;
}

Identifier::~Identifier()
{
//    delete tok_;
}


PrefixExpression::~PrefixExpression()
{
    if (right_) delete right_;
    //TODO dealloc tok!
    //delete tok_;
}


InfixExpression::~InfixExpression()
{
    delete lhs_;
    //delete tok_;
    delete rhs_;
}

InfixExpression::InfixExpression(Token *tok, Node *lhs, Node *rhs)
     : Node(AST_INFIXEXPRESSION), tok_(tok), lhs_(lhs), op_(tok->type()), rhs_(rhs)
 {
 }


std::string InfixExpression::asString() const
 {
     std::string ret;

     ret.append("(");
     ret.append(lhs_->asString());
     ret.append(" " + std::string(tok_names[op_]) + " ");
     if (rhs_) {
         ret.append(rhs_->asString());
     }
     ret.append(")");

     return ret;
 }


std::string PrefixExpression::asString() const
 {
     std::string ret;

     ret.append("(");
     ret.append(operat_);
     if (right_) {
         ret.append(right_->asString());
     }
     ret.append(")");

     return ret;
 }




Return::~Return()
{
    if (returnValue_) delete returnValue_;
}

std::string Identifier::asString() const
 {
     return value_;
 }

std::string Let::asString() const
 {
     std::string ret;
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


std::string IntegerLiteral::asString() const
{
    std::stringstream ss;
    ss << value_;
    return ss.str();
}


std::string Return::asString() const
 {
     std::string ret;
     ret.append(tokenLiteral() + " ");

     if (returnValue_) { // TODO remove condition in the future
         ret.append(returnValue_->asString());
     }

     ret.append(";");

     return ret;
 }

std::string ExpressionStatement::asString() const
 {
     if (expression_) { //TODO remove condition in the future
         return expression_->asString();
     }
 
     return "";
 }

std::string Program::asString() const
 {
     std::string ret;
 
     for (auto &a : statements_)
     {
         ret.append(a->asString());
     }
 
     return ret;
 }



BlockStatement::~BlockStatement()
{
    for (auto &stmt : statements_) {
        delete stmt;
    }
}


FunctionLiteral::~FunctionLiteral()
{
     for (auto &param : parameters_) {
         delete param;
     }
     delete body_;
}

std::string If::asString() const
 {
     std::string ret;
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


std::string FunctionLiteral::asString() const
 {
     std::string ret;
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



 
std::string BlockStatement::asString() const
 {
     std::string ret;
 
     for (auto &a : statements_) {
         ret.append(a->asString());
     }
 
     return ret;
 }




 
 CallExpression::~CallExpression()
 {
     for (auto &arg : arguments_) {
         delete arg;
     }
     delete function_;
 }

std::string CallExpression::asString() const
 {
     std::string ret;
     std::vector<std::string> args;

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

void IntegerLiteral::accept(Visitor &v)
{
    v.visitIntegerLiteral(this);
}


void Program::accept(Visitor &v)
{
    v.visitProgram(this);
}

void ExpressionStatement::accept(Visitor &v)
{
    v.visitExpressionStatement(this);
}


void Boolean::accept(Visitor &v)
{
    v.visitBoolean(this);
}


void PrefixExpression::accept(Visitor &v) {
    v.visitPrefixExpression(this);
}

void InfixExpression::accept(Visitor &v) {
    v.visitInfixExpression(this);
}

void If::accept(Visitor &v) {
    v.visitIfExpression(this);
}

void BlockStatement::accept(Visitor &v) {
    v.visitBlockStatement(this);
}

void Return::accept(Visitor &v) {
    v.visitReturn(this);
}

void Let::accept(Visitor &v) {
    v.visitLet(this);
}


void Identifier::accept(Visitor &v) {
    v.visitIdentifier(this);
}

void FunctionLiteral::accept(Visitor &v) {
    v.visitFunctionLiteral(this);
}

void CallExpression::accept(Visitor &v) {
    v.visitCallExpression(this);
}


Single *IntegerLiteral::evalIntegerLiteral(Environment *s) {
    return Single::alloc(value());
}

Single *IntegerLiteral::eval(Environment *s) {
    return evalIntegerLiteral(s);
}

Single *Program::evalProgram(Environment *s) {
    Single *ret = nullptr;
    for (auto &stmt : statements()) {
        if (ret) ret->release();

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

Single *Program::eval(Environment *s) {
    return evalProgram(s);
}

Single *Identifier::evalIdentifier(Environment *s) {
    const std::string &key = value();
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

Single *Identifier::eval(Environment *s) {
   return evalIdentifier(s); 
}

Single *Let::eval(Environment *s) {
    return evalLet(s);
}

Single *Let::evalLet(Environment *s) {
    Single *val = value()->eval(s);
    if (isError(val)) {
        return val;
    }
    s->set(name()->value(), val);
    return val;
}


Single *Return::eval(Environment *s) {
    return evalReturn(s);
}

Single *Return::evalReturn(Environment *s) {
    Single *ret = value()->eval(s);
    if (isError(ret)) {
        return ret;
    }
    return Single::alloc(ret);
}

Single *ExpressionStatement::eval(Environment *s) {
    return evalExpressionStatement(s);
}

Single *ExpressionStatement::evalExpressionStatement(Environment *s) {
    return expression()->eval(s);
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

 static Single *evalMinusPrefixOperatorExpression(Single *right)
 {
     if (right->type_ != INTEGER) {
         char buffer[80];
         sprintf(buffer, "unknown operator: -%s", object_name[right->type_]);
         return Single::alloc(buffer);
     }
     return Single::alloc(-right->data.integer.value_);
 }





static Single *evalPrefixExpression_core(const std::string &op, Single *right) {
    Single *ret = nullptr;
    if (!op.compare("!")) {
        ret = evalBangOperatorExpression(right);
    } else if (!op.compare("-")) {
        ret = evalMinusPrefixOperatorExpression(right);
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s%s", op.c_str(), object_name[right->type_] );
        ret =  Single::alloc(buffer);
    }
    if (right) {
        right->release();
    }
    return ret;
}



Single *PrefixExpression::eval(Environment *s) {
    return evalPrefixExpression(s);
}

Single *PrefixExpression::evalPrefixExpression(Environment *s) {
    Single *r = right()->eval(s);
    if (isError(r)) {
        return r;
    }
    return evalPrefixExpression_core(operator_s(), r);
}


static Single *evalStringInfixExpression(TokenType op, Single *left, Single *right) {
    if (op != T_PLUS) {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_], tok_names[op], object_name[right->type_]);
        return Single::alloc(buffer);
    }

    char buffer[1024];
    const char *l = left->data.string.value_;
    const char *r = right->data.string.value_;
    strcpy(buffer, l);
    strcat(buffer, r);
    return Single::alloc(buffer, STRING);
}

static Single *evalIntegerInfixExpression(TokenType op, Single *left, Single *right) {
    //TODO remove all those compare with an array of function callbacks
    Single *temp = nullptr;

    if (op == T_PLUS) {
        temp = Single::alloc(left->data.integer.value_ + right->data.integer.value_);
    } else if (op == T_MINUS) {
        temp = Single::alloc(left->data.integer.value_ - right->data.integer.value_);
    } else if (op == T_ASTERISK) {
        temp = Single::alloc(left->data.integer.value_ * right->data.integer.value_);
    } else if (op == T_SLASH) {
        temp = Single::alloc(left->data.integer.value_ / right->data.integer.value_);
    } else if (op == T_LT) {
        temp = nativeBoolToSingObj(left->data.integer.value_ < right->data.integer.value_);
    } else if (op == T_GT) {
        temp = nativeBoolToSingObj(left->data.integer.value_ > right->data.integer.value_);
    } else if (op == T_EQ) {
        temp = nativeBoolToSingObj(left->data.integer.value_ == right->data.integer.value_);
    } else if (op == T_NOT_EQ) {
        temp = nativeBoolToSingObj(left->data.integer.value_ != right->data.integer.value_);
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_], tok_names[op], object_name[right->type_]);
        temp = Single::alloc(buffer);
    }

    return temp;
}


static Single *evalInfixExpression_core(TokenType op, Single *left, Single *right) {
    Single *temp = nullptr;
    if (left->type_ == INTEGER && right->type_ == INTEGER) {
        temp = evalIntegerInfixExpression(op, left, right);
    
    } else if (op == T_EQ) {
        temp = nativeBoolToSingObj(left == right);
    } else if (op == T_NOT_EQ) {
       temp = nativeBoolToSingObj(left != right);
    } else if (left->type_ != right->type_) {
        char buffer[80];
        sprintf(buffer, "type mismatch: %s %s %s", object_name[left->type_], tok_names[op], object_name[right->type_]);
        temp = Single::alloc(buffer);
    } else if (left->type_ == STRING) { 
        temp = evalStringInfixExpression(op, left, right);
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_], tok_names[op], object_name[right->type_]);
        temp = Single::alloc(buffer);
    }

    if (temp) {
       left->release(); 
       right->release(); 
    }
    return temp;
}



Single *InfixExpression::eval(Environment *s) {
    return evalInfixExpression(s);
}

Single *InfixExpression::evalInfixExpression(Environment *s) {
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



Single *Boolean::eval(Environment *s) {
    return evalBoolean(s);
}

Single *Boolean::evalBoolean(Environment *s) {
    return nativeBoolToSingObj(value());
}

Single *BlockStatement::evalBlockStatement(Environment *store) {
    Single *temp = nullptr;
    for (auto &s : statements()) {
        if (temp)  temp->release();
        temp = s->eval(store);
        if (temp && (temp->type_ == RETURN || temp->type_ == ERROR)) {
            return temp;
        }
    }
    return temp;
}

Single *BlockStatement::eval(Environment *s) {
    return evalBlockStatement(s);
}


Single *If::eval(Environment *s) {
    return evalIf(s);
}

Single *If::evalIf(Environment *s) {
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
    return evalFunctionLiteral(s);
}

Single *FunctionLiteral::evalFunctionLiteral(Environment *s) {
    std::vector<Identifier *> &params = parameters();
    BlockStatement *b = body();
    return Single::alloc(&params, s, b);
}


static bool evalExpressions(const std::vector<Node *> &args, std::array<Single *, MAX_ARGS_NUM> &arg_to_return, Environment *s) {

    size_t numArgs = args.size();

    if (numArgs > MAX_ARGS_NUM) {
        char buffer[80];
        sprintf(buffer, "Cannot support more than 25 args in a function");
        Single *err =  Single::alloc(buffer);
        arg_to_return[0] = err;
        return true;
    }

    for (size_t i = 0; i < numArgs; i++) {
        Single *evaluated = args[i]->eval(s);
        if (isError(evaluated)) {
            //FIXME! mem leak here! Needs release osa mpikan sto arg_to_return prin ftaso edo.
            arg_to_return[0] = evaluated;
            return true;
        }
        arg_to_return[i] = evaluated;
    } 

    return false;
}


Environment *extendFunctionEnv(Single *fn, std::array<Single *, MAX_ARGS_NUM> &args)
{
    Environment *env = Environment::alloc(fn->data.function.env_);//FIXME deallocate?
    std::vector<Identifier *> &params = *(fn->data.function.parameters_);
    for (size_t i = 0; i < params.size(); i++) {
        Identifier *iden = params[i];
        env->set(iden->value(), args[i]);
    }

    return env;
}


//OK
static Single *unwrapReturnValue(Single *val) {
    if (val->type_ != RETURN) {
        return val;
    }
    Single *result = val->data.obj.obj_;
    val->release();
    return result;
}



static Single *applyFunction(Single *fn, std::array<Single *, MAX_ARGS_NUM> &args, size_t args_num) {
    if (fn->type_ == BUILTIN) {
        return fn->data.builtin.f_(args, args_num);
    }
   
    if (fn->type_ == FUNCTION) {
        Environment *extended_env = extendFunctionEnv(fn, args);
        Single *ret = fn->data.function.body_->eval(extended_env);

        ret = unwrapReturnValue(ret);
        extended_env->release();
        return ret;
    }

    char buffer[80];
    sprintf(buffer, "not a function: %s\n", object_name[fn->type_]);
    return Single::alloc(buffer);

}


Single *CallExpression::eval(Environment *s) {
    return evalCallExpression(s);
}

Single *CallExpression::evalCallExpression(Environment *s) {
    Single *fn = function()->eval(s);
    if (isError(fn)) {
        return fn;
    }
    std::array<Single *, MAX_ARGS_NUM> applied_args;
    const std::vector<Node *> &args = arguments();
    bool isError = evalExpressions(args, applied_args, s);
    if (isError) {
        return applied_args[0];
    }
    //delete fn
    Single *ret = applyFunction(fn, applied_args, args.size());
    for (size_t i = 0; i < size(); i++) {
        applied_args[i]->release();
    }
    fn->release();
    return ret;
}


std::string StringLiteral::asString() const {
    return tok_->literal();
}

std::string ArrayLiteral::asString() const {
    std::string ret;
    
    ret.append("[");

    //TODO refactoring. I need the index during the loop
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
    for (auto &a: elements_) {
        delete a;
    }
}


std::string IndexExpression::asString() const
{
    std::string ret;

    ret.append("(");
    ret.append(left_->asString());
    ret.append("[");
    ret.append(index_->asString());
    ret.append("])");

    return ret;
}


Single *ArrayLiteral::eval(Environment *s) {
    std::array<Single *, MAX_ARGS_NUM> eval_elems_; // TODO fix me. This must be a vector;
    const std::vector<Node *> &elems = elements();
    bool isError = evalExpressions(elems, eval_elems_, s);
    if (isError) {
        return eval_elems_[0];
    }
    return Single::alloc(&eval_elems_[0], elems.size());
}


static Single *evalArrayIndexExpression(Single *left, Single *index) {
    int idx = index->data.integer.value_;
    int max = left->data.array.num_ - 1;
    if (idx < 0 || idx > max) {
        return &Model::null_o;
    }
    Single *ret = left->data.array.elems_[idx];
    ret->retain();
    return ret;
}

static Single *evalIndexExpression(Single *left, Single *index) {
    if (left->type_ == ARRAY && index->type_ == INTEGER) {
        return evalArrayIndexExpression(left, index);
    } else {
        char buffer[80];
        sprintf(buffer, "index operator not supported: %s", object_name[left->type_]);
        return Single::alloc(buffer);
    }
}

Single *IndexExpression::eval(Environment *s) {
    Single *l= left()->eval(s);
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


std::string HashLiteral::asString() const
{
    std::string ret;
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


