#include <sstream>
#include <string.h>
#include "ast.hpp"
#include "env.hpp"
#include "visitor.hpp"

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
     : tok_(tok), lhs_(lhs), op_(tok->literal()), rhs_(rhs)
 {
 }


std::string InfixExpression::asString() const
 {
     std::string ret;

     ret.append("(");
     ret.append(lhs_->asString());
     ret.append(" " + op_ + " ");
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


Single *IntegerLiteral::eval(Environment *s) {
    return new Single(value());
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

static Single *evalIdentifier(const std::string &key, Environment *s) {
    Single *val = s->get(key);
    if (!val) {
        char buffer[80];
        sprintf(buffer, "identifier not found: %s", key.c_str());
        return new Single(strdup(buffer));
    }
    val->retain();
    return val;
}

Single *Identifier::eval(Environment *s) {
   return evalIdentifier(value(), s); 
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
    return new Single(ret);
}

Single *ExpressionStatement::eval(Environment *s) {
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
         return new Single(strdup(buffer));
     }
     return new Single(-right->data.integer.value_);
 }





static Single *evalPrefixExpression(const std::string &op, Single *right) {
    Single *ret = nullptr;
    if (!op.compare("!")) {
        ret = evalBangOperatorExpression(right);
    } else if (!op.compare("-")) {
        ret = evalMinusPrefixOperatorExpression(right);
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s%s", op.c_str(), object_name[right->type_] );
        ret =  new Single(strdup(buffer));
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
    return evalPrefixExpression(operator_s(), r);
}


static Single *evalIntegerInfixExpression(const std::string &op, Single *left, Single *right) {
    //TODO remove all those compare with an array of function callbacks
    Single *temp = nullptr;

    if (!op.compare("+")) {
        temp = new Single(left->data.integer.value_ + right->data.integer.value_);
    } else if (!op.compare("-")) {
        temp = new Single(left->data.integer.value_ - right->data.integer.value_);
    } else if (!op.compare("*")) {
        temp = new Single(left->data.integer.value_ * right->data.integer.value_);
    } else if (!op.compare("/")) {
        temp = new Single(left->data.integer.value_ / right->data.integer.value_);
    } else if (!op.compare("<")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ < right->data.integer.value_);
    } else if (!op.compare(">")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ > right->data.integer.value_);
    } else if (!op.compare("==")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ == right->data.integer.value_);
    } else if (!op.compare("!=")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ != right->data.integer.value_);
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_], op.c_str(), object_name[right->type_]);
        temp = new Single(strdup(buffer));
    }

    return temp;
}


Single *evalInfixExpression(const std::string &op, Single *left, Single *right) {
    Single *temp = nullptr;
    if (left->type_ == INTEGER && right->type_ == INTEGER) {
        temp = evalIntegerInfixExpression(op, left, right);
    } else if (!op.compare("==")) {
        temp = nativeBoolToSingObj(left == right);
    } else if (!op.compare("!=")) {
       temp = nativeBoolToSingObj(left != right);
    } else if (left->type_ != right->type_) {
        char buffer[80];
        sprintf(buffer, "type mismatch: %s %s %s", object_name[left->type_], op.c_str(), object_name[right->type_]);
        temp = new Single(strdup(buffer));
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_], op.c_str(), object_name[right->type_]);
        temp = new Single(strdup(buffer));
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
    Single *ret = evalInfixExpression(op(), l, r);

    return ret;
}


Single *Boolean::eval(Environment *s) {
    return nativeBoolToSingObj(value());
}

Single *evalBlockStatement(BlockStatement *a, Environment *store) {
    Single *temp = nullptr;
    for (auto &s : a->statements()) {
        if (temp)  temp->release();
        temp = s->eval(store);
        if (temp && (temp->type_ == RETURN || temp->type_ == ERROR)) {
            return temp;
        }
    }
    return temp;
}

Single *BlockStatement::eval(Environment *s) {
    return evalBlockStatement(this, s);
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
    std::vector<Identifier *> &params = parameters();
    BlockStatement *b = body();
    return new Single(&params, s, b);
}


void evalExpressions(const std::vector<Node *> &args, std::vector<Single *> &arg_to_return, Environment *s) {
    for (size_t i = 0; i < args.size(); i++) {
        Single *evaluated = args[i]->eval(s);
        if (isError(evaluated)) {
            arg_to_return.resize(1);
            arg_to_return[0] = evaluated;
            return;
        }
        arg_to_return[i] = evaluated;
    } 
}


Environment *extendFunctionEnv(Single *fn, std::vector<Single *> &args)
{
    Environment *env = new Environment(fn->data.function.env_);//FIXME deallocate?
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



static Single *applyFunction(Single *fn, std::vector<Single *> &args, Environment *s) {
    if (fn->type_ != FUNCTION) {
        char buffer[80];
        sprintf(buffer, "not a function: %s\n", object_name[fn->type_]);
        return new Single(strdup(buffer));
    }
    

    Environment *extended_env = extendFunctionEnv(fn, args);
    Single *ret = fn->data.function.body_->eval(extended_env);

    //To extended env de xrisimopoieitai allo.
    //Giauto to svino.
    //Mporei omos na mou gurnaei metavliti apo to extendedn env.
    // opote tin kano unused kai svino to env.
    ret = unwrapReturnValue(ret);
    extended_env->release();
    //ret->used_ = false;
    //extended_env->erase(ret);
    //delete extended_env;
    return ret;
}

Single *CallExpression::eval(Environment *s) {
    Single *fn = function()->eval(s);
    if (isError(fn)) {
        return fn;
    }
    std::vector<Single *> args(size());
    evalExpressions(arguments(), args, s);
    if (args.size() == 1 && isError(args[0])) {
        return args[0];
    }
    //delete fn
    Single *ret = applyFunction(fn, args, s);
    for (auto &a : args) {
        a->release();
    }
    fn->release();
    return ret;
}

