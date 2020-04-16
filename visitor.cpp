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
    ret_ = new Integer(a->value());
}

void Evaluator::visitExpressionStatement(ExpressionStatement *a)
{
    a->expression()->accept(*this);
}
