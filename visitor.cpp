#include "visitor.hpp"
#include "object.hpp"


void Evaluator::evalStatements(const std::vector<Node *> &statements) {
    for (auto &s : statements) {
        s->accept(*this);
    }
}

void Evaluator::visitProgram(Program *a) {
    const std::vector<Node *> &statements = a->statements();
    evalStatements(statements);
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
    if (ret_ && obj != ret_ && ret_ != &Model::false_o && ret_ != &Model::true_o && ret_ != &Model::null_o) {
        delete ret_;
    }
    ret_ = obj;
}


Single *Evaluator::setResultNull() {
    Single *temp = ret_;
    ret_ = nullptr;
    return temp;
}

 void Evaluator::evalMinusPrefixOperatorExpression()
 {
     if (ret_->type_ != INTEGER) {
         setResult(&Model::null_o);
     }
    ret_->data.integer.value_ = -ret_->data.integer.value_;
 }


void Evaluator::evalPrefixExpression(const std::string &op) {
    if (!op.compare("!")) {
        evalBangOperatorExpression();
    } else if (!op.compare("-")) {
        evalMinusPrefixOperatorExpression();
    } else {
        setResult(&Model::null_o);
    }
}

void Evaluator::visitPrefixExpression(PrefixExpression *a) {
    a->right()->accept(*this);
    evalPrefixExpression(a->operator_s());
}


void Evaluator::evalIntegerInfixExpression(const std::string &op, Single *left, Single *right) {
    //TODO remove all those compare with an array of function callbacks
    Single *temp = nullptr;

    if (!op.compare("+")) {
        left->data.integer.value_ += right->data.integer.value_;
    } else if (!op.compare("-")) {
        left->data.integer.value_ -= right->data.integer.value_;
    } else if (!op.compare("*")) {
        left->data.integer.value_ *= right->data.integer.value_;
    } else if (!op.compare("/")) {
        left->data.integer.value_ /= right->data.integer.value_;
    } else if (!op.compare("<")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ < right->data.integer.value_);
    } else if (!op.compare(">")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ > right->data.integer.value_);
    } else if (!op.compare("==")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ == right->data.integer.value_);
    } else if (!op.compare("!=")) {
        temp = nativeBoolToSingObj(left->data.integer.value_ != right->data.integer.value_);
    } else {
        setResult(&Model::null_o);
    }

    if (temp) {
        delete left;
        setResult(temp);
    } else {
        setResult(left);
    }
    delete right;
}

void Evaluator::evalInfixExpression(const std::string &op, Single *left, Single *right) {
    if (left->type_ == INTEGER && right->type_ == INTEGER) {
        evalIntegerInfixExpression(op, left, right);
    } else if (!op.compare("==")) {
        setResult(nativeBoolToSingObj(left == right));
    } else if (!op.compare("!=")) {
       setResult(nativeBoolToSingObj(left != right));
    } else {
        setResult(&Model::null_o);
    }
}


void Evaluator::visitInfixExpression(InfixExpression *a) {
    a->lhs()->accept(*this);
    Single *left = setResultNull();
    a->rhs()->accept(*this);
    Single *right = setResultNull();
    evalInfixExpression(a->op(), left, right);
}
