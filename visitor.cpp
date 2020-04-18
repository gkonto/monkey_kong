#include <string.h>
#include "visitor.hpp"
#include "object.hpp"
#include "env.hpp"


//FIXME Delete if not used
void Evaluator::evalStatements(const std::vector<Node *> &statements) {
    for (auto &s : statements) {
        s->accept(*this);
        if (ret_->type_ == RETURN) {
            setResult(ret_->data.obj.obj_);
            return;
        }
    }
}

void Evaluator::evalBlockStatement(BlockStatement *a) {
    for (auto &s : a->statements()) {
        s->accept(*this);
        if (ret_ && (ret_->type_ == RETURN || ret_->type_ == ERROR)) {
            return;
        }
    }
}

void Evaluator::evalProgram(Program *a) {
    for (auto &s : a->statements()) {
        s->accept(*this);
        if (ret_ && ret_->type_ == RETURN) {
            setResult(ret_->data.obj.obj_);
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
    setResult(new Single(a->value()));
}

void Evaluator::visitExpressionStatement(ExpressionStatement *a) {
    a->expression()->accept(*this);
}

void Evaluator::visitBoolean(Boolean *a) {
    setResult(nativeBoolToSingObj(a->value()));
}


Single *Evaluator::nativeBoolToSingObj(bool input) {
    return input ? &Model::true_o : &Model::false_o;
}

void Evaluator::evalBangOperatorExpression() {
    if (ret_ == &Model::true_o) {
        setResult(&Model::false_o);
    } else if (ret_ == &Model::false_o) {
        setResult(&Model::true_o);
    } else if (ret_ == &Model::null_o) {
        setResult(&Model::null_o);
    } else {
        setResult(&Model::false_o);
    } 
}

void Evaluator::setResult(Single *obj) {
    if (ret_ && obj != ret_) {
        conditionalDelete(ret_);
    }
    ret_ = obj;
}


Single *Evaluator::setResultNull() {
    Single *temp = ret_;
    ret_ = nullptr;
    return temp;
}

void Evaluator::conditionalDelete(Single *del_ent) {
    if (!del_ent->used_ &&
         del_ent != &Model::false_o && 
         del_ent != &Model::true_o &&  
         del_ent != &Model::null_o) {
        delete del_ent;
    }
}

 void Evaluator::evalMinusPrefixOperatorExpression()
 {
     if (ret_->type_ != INTEGER) {
         char buffer[80];
         sprintf(buffer, "unknown operator: -%s", object_name[ret_->type_]);
         setResult(new Single(strdup(buffer)));
         return;
     }
    ret_->data.integer.value_ = -ret_->data.integer.value_;
 }


void Evaluator::evalPrefixExpression(const std::string &op) {
    if (!op.compare("!")) {
        evalBangOperatorExpression();
    } else if (!op.compare("-")) {
        evalMinusPrefixOperatorExpression();
    } else {
        char buffer[80];
        sprintf(buffer, "unknown operator: %s%s", op.c_str(), object_name[ret_->type_] );
        setResult(new Single(strdup(buffer)));
    }
}

void Evaluator::visitPrefixExpression(PrefixExpression *a) {
    a->right()->accept(*this);
    if (isError(ret_)) {
        return;
    }
    evalPrefixExpression(a->operator_s());
}


void Evaluator::evalIntegerInfixExpression(const std::string &op, Single *left, Single *right) {
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

    if (temp) {
        setResult(temp);
    }
    conditionalDelete(right);
    conditionalDelete(left);
}

void Evaluator::evalInfixExpression(const std::string &op, Single *left, Single *right) {
    Single *temp = nullptr;
    if (left->type_ == INTEGER && right->type_ == INTEGER) {
        evalIntegerInfixExpression(op, left, right);
    } else if (!op.compare("==")) {
        setResult(nativeBoolToSingObj(left == right));
    } else if (!op.compare("!=")) {
       setResult(nativeBoolToSingObj(left != right));
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
       conditionalDelete(left); 
       conditionalDelete(right); 
       setResult(temp);
    }
}


void Evaluator::visitInfixExpression(InfixExpression *a) {
    a->lhs()->accept(*this);
    if (isError(ret_)) {
        return;
    }
    Single *left = setResultNull();
    a->rhs()->accept(*this);

    if (isError(ret_)) {
        return;
    }
    Single *right = setResultNull();
    evalInfixExpression(a->op(), left, right);
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
    Single *cond = setResultNull();
    if (isTruthy(cond)) {
        a->consequence()->accept(*this);
    } else if (a->alternative()) {
        a->alternative()->accept(*this);
    } else {
        setResult(&Model::null_o);
    }

    conditionalDelete(cond);
}

void Evaluator::visitReturn(Return *a) {
    a->value()->accept(*this);
    if (isError(ret_)) {
        return;
    }
    Single *val = setResultNull();
    setResult(new Single(val));
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
    Single *val = setResultNull();
    env_->set(a->name()->value(), val);
}

void Evaluator::evalIdentifier(Identifier *a) {
    Single *val = env_->get(a->value());
    if (!val) {
        char buffer[80];
        sprintf(buffer, "identifier not found: %s", a->value().c_str());
        setResult(new Single(strdup(buffer)));
        return;
    }
    setResult(val);
}

void Evaluator::visitIdentifier(Identifier *a) {
    evalIdentifier(a);
}


void Evaluator::visitFunctionLiteral(FunctionLiteral *a) {
    Single *o = new Single(&a->parameters(), env_, a->body());
    setResult(o);
}

void Evaluator::evalExpressions(const std::vector<Node *> &args, Environment *env, std::vector<Single *> &arg_to_return) {
    for (size_t i = 0; i < args.size(); i++) {
        args[i]->accept(*this);
        Single *evaluated = setResultNull();
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
        setResult(new Single(strdup(buffer)));
        return;
    }
    

    Environment *extendedEnv = extendFunctionEnv(fn, args);
    env_ = extendedEnv;
    fn->data.function.body_->accept(*this);
    env_ = extendedEnv->outer();
    unwrapReturnValue();

    extendedEnv->erase(ret_);
    ret_->used_ = false;

    if (!fn->used_) {
        DeleteSingle(fn);
    }
    delete extendedEnv;
}

void Evaluator::unwrapReturnValue() {
    if (ret_->type_ != RETURN) {
        return;
    }
    setResult(ret_->data.obj.obj_);
}



Environment *Evaluator::extendFunctionEnv(Single *fn, std::vector<Single *> &args)
{
    Environment *env = new Environment(env_);//FIXME deallocate?
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

    Single *fn = setResultNull();
    std::vector<Single *> args(a->size());
    evalExpressions(a->arguments(), env_, args);
    if (args.size() == 1 && isError(args[0])) {
        setResult(args[0]);
    }
    applyFunction(fn, args);
}


