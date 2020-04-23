#include <string.h>
#include "visitor.hpp"
#include "object.hpp"
#include "env.hpp"


void Evaluator::attach(Single *o) {
    ret_ = o;
}

Single *Evaluator::detach() {
    Single *temp = ret_;
    ret_ = 0;
    return temp;
}

void Evaluator::release() {
    if (ret_) {
        ret_->release();
    }
    ret_ = 0;
}


//FIXME Delete if not used
void Evaluator::evalStatements(const std::vector<Node *> &statements) {
    for (auto &s : statements) {
        s->accept(*this);
        if (ret_->type_ == RETURN) {
            Single *temp = ret_;
            ret_ = temp->data.obj.obj_;
            temp->release();
            return;
        }
    }
}

void Evaluator::evalBlockStatement(BlockStatement *a) {
    for (auto &s : a->statements()) {
//        if (ret_) ret_->release();
        s->accept(*this);
        if (ret_ && (ret_->type_ == RETURN || ret_->type_ == ERROR)) {
            return;
        }
    }
}

void Evaluator::evalProgram(Program *a) {
    for (auto &s : a->statements()) {
        release();
        s->accept(*this);
        if (ret_ && ret_->type_ == RETURN) {
            Single *temp = ret_;
            ret_ = ret_->data.obj.obj_;
            temp->release();
            return;
        }
        if (ret_ && ret_->type_ == ERROR) {
            return;
        }
    }
}


void Evaluator::visitProgram(Program *a) {
    evalProgram(a);
}

void Evaluator::visitIntegerLiteral(IntegerLiteral *a) {
    release();
    ret_ = new Single(a->value());
}

void Evaluator::visitExpressionStatement(ExpressionStatement *a) {
    a->expression()->accept(*this);
}

void Evaluator::visitBoolean(Boolean *a) {
    release();
    ret_ = nativeBoolToSingObj(a->value());
}


Single *Evaluator::nativeBoolToSingObj(bool input) {
    return input ? &Model::true_o : &Model::false_o;
}

void Evaluator::evalBangOperatorExpression() {
    Single *temp = ret_;
    if (ret_ == &Model::true_o) {
        ret_ = &Model::false_o;
    } else if (ret_ == &Model::false_o) {
        ret_ = &Model::true_o;
    } else if (ret_ == &Model::null_o) {
        ret_ = &Model::null_o;
    } else {
        ret_ = &Model::false_o;
    } 
    temp->release();
}

 void Evaluator::evalMinusPrefixOperatorExpression()
 {
     if (ret_->type_ != INTEGER) {
         char buffer[80];
         sprintf(buffer, "unknown operator: -%s", object_name[ret_->type_]);

         ret_->release();
         ret_ = new Single(strdup(buffer));
         return;
     }
     Single *temp = ret_;
     ret_ = new Single(-ret_->data.integer.value_);
     temp->release();
 }


void Evaluator::evalPrefixExpression(const std::string &op) {
    if (!op.compare("!")) {
        evalBangOperatorExpression();
    } else if (!op.compare("-")) {
        evalMinusPrefixOperatorExpression();
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s%s", op.c_str(), object_name[ret_->type_] );
        ret_->release();
        ret_ = new Single(strdup(buffer));
    }
}

void Evaluator::visitPrefixExpression(PrefixExpression *a) {
    a->right()->accept(*this);
    if (isError(ret_)) {
        return;
    }
    evalPrefixExpression(a->operator_s());
}


Single *Evaluator::evalIntegerInfixExpression(TokenType op, Single *left, Single *right) {
    //TODO remove all those compare with an array of function callbacks
    Single *temp = nullptr;

    if (op == T_PLUS) {
        temp = new Single(left->data.integer.value_ + right->data.integer.value_);
    } else if (op == T_MINUS) {
        temp = new Single(left->data.integer.value_ - right->data.integer.value_);
    } else if (op == T_ASTERISK) {
        temp = new Single(left->data.integer.value_ * right->data.integer.value_);
    } else if (op == T_SLASH) {
        temp = new Single(left->data.integer.value_ / right->data.integer.value_);
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
        temp = new Single(strdup(buffer));
    }

    return temp;
}

Single *Evaluator::evalInfixExpression(TokenType op, Single *left, Single *right) {
    Single *temp = nullptr;
    if (left->type_ == INTEGER && right->type_ == INTEGER) {
        temp = evalIntegerInfixExpression(op, left, right);
    } else if (op == T_EQ) {
        temp = nativeBoolToSingObj(left==right);
    } else if (op == T_NOT_EQ) {
        temp = nativeBoolToSingObj(left!=right);
    } else if (left->type_ != right->type_) {
        char buffer[80];
        sprintf(buffer, "type mismatch: %s %s %s", object_name[left->type_], tok_names[op], object_name[right->type_]);
        temp = new Single(strdup(buffer));
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s %s %s", object_name[left->type_], tok_names[op], object_name[right->type_]);
        temp = new Single(strdup(buffer));
    }

    return temp;
}


void Evaluator::visitInfixExpression(InfixExpression *a) {
    a->lhs()->accept(*this);
    if (isError(ret_)) {
        return;
    }
    Single *left = detach();
    a->rhs()->accept(*this);

    if (isError(ret_)) {
        return;
    }
    Single *right = detach();
    Single *temp = evalInfixExpression(a->op(), left, right);
    release();
    attach(temp);

    if (left) left->release();
    if (right) right->release();
}


void Evaluator::visitBlockStatement(BlockStatement *a) {
    evalBlockStatement(a);
}


bool Evaluator::isTruthy(Single *obj) const {
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


void Evaluator::visitIfExpression(If *a) {
    a->condition()->accept(*this);

    if (isError(ret_)) {
        return;
    }
    Single *cond = detach();
    if (isTruthy(cond)) {
        a->consequence()->accept(*this);
    } else if (a->alternative()) {
        a->alternative()->accept(*this);
    } else {
        ret_ = &Model::null_o;
    }

    cond->release();
}

void Evaluator::visitReturn(Return *a) {
    a->value()->accept(*this);
    if (isError(ret_)) {
        return;
    }
    ret_ = new Single(ret_);
}


bool Evaluator::isError(Single *val) const {
    if (val) {
        return val->type_ == ERROR;
    }
    return false;
} 

void Evaluator::visitLet(Let *a) {
    a->value()->accept(*this);
    if (isError(ret_)) {
        return;
    }
    env_->set(a->name()->value(), ret_);
}

void Evaluator::evalIdentifier(Identifier *a) {
    Single *val = env_->get(a->value());
    if (!val) {
        char buffer[80];
        sprintf(buffer, "identifier not found: %s", a->value().c_str());
        release();
        attach(new Single(strdup(buffer)));
        return;
    }
    val->retain();
    attach(val);
}

void Evaluator::visitIdentifier(Identifier *a) {
    evalIdentifier(a);
}


void Evaluator::visitFunctionLiteral(FunctionLiteral *a) {
    Single *o = new Single(&a->parameters(), env_, a->body());
    //release();
    ret_ = o;
}

void Evaluator::evalExpressions(const std::vector<Node *> &args, Environment *env, std::vector<Single *> &arg_to_return) {
    for (size_t i = 0; i < args.size(); i++) {
        //release();
        args[i]->accept(*this);
        Single *evaluated = detach();
        if (isError(evaluated)) {
            arg_to_return.resize(1);
            arg_to_return[0] = evaluated;
            return;
        }
        arg_to_return[i] = evaluated;
    } 
}

void Evaluator::applyFunction(Single *fn, std::vector<Single *> &args) {
    if (fn->type_ != FUNCTION) {
        char buffer[80];
        sprintf(buffer, "not a function: %s\n", object_name[fn->type_]);
        ret_->release();
        ret_ = new Single(strdup(buffer));
        return;
    }
    

    Environment *extendedEnv = extendFunctionEnv(fn, args);
    Environment *temp = env_;
    env_ = extendedEnv;

    fn->data.function.body_->accept(*this);
    env_ = temp;
    extendedEnv->release();
    unwrapReturnValue();
}

void Evaluator::unwrapReturnValue() {
    if (ret_->type_ != RETURN) {
        return;
    }
    Single *temp = ret_;
    ret_ = ret_->data.obj.obj_;
    temp->release();
}



Environment *Evaluator::extendFunctionEnv(Single *fn, std::vector<Single *> &args)
{
    Environment *env = new Environment(fn->data.function.env_);//FIXME deallocate?
    std::vector<Identifier *> &params = *(fn->data.function.parameters_);
    for (size_t i = 0; i < params.size(); i++) {
        Identifier *iden = params[i];
        env->set(iden->value(), args[i]);
    }

    return env;
}



void Evaluator::visitCallExpression(CallExpression *a) {
    a->function()->accept(*this);
    if (isError(ret_)) {
        return;
    }

    Single *fn = detach();
    std::vector<Single *> args(a->size());
    evalExpressions(a->arguments(), env_, args);
    if (args.size() == 1 && isError(args[0])) {
        ret_ = args[0];
    }
    applyFunction(fn, args);
    for (auto &a : args) {
        a->release();
    }
    fn->release();
}


