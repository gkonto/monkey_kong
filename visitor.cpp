#include "visitor.hpp"
#include "ast.hpp"
#include <iostream>
#include <vector>


void VariableNumberingVisitor::visitProgram(Program *p) 
{
    for (auto s : *p) {
        s->accept(this);
    }
}

void VariableNumberingVisitor::visitIdentifier(Identifier *p) 
{
    if (!params_) return;

    for (size_t i = 0; i < params_->size(); ++i) {
        if (!p->value().compare(params_->at(i)->value())) {
            p->setIndex(i);
        }
    }
}

void VariableNumberingVisitor::visitLet(Let *p) 
{
    auto iden = p->name();
    iden->accept(this);
    auto val = p->value();
    val->accept(this);
}

void VariableNumberingVisitor::visitReturn(Return *p) 
{
}

void VariableNumberingVisitor::visitExpressionStatement(ExpressionStatement *p)
{
    p->expression()->accept(this);
}

void VariableNumberingVisitor::visitIntegerLiteral(IntegerLiteral *p)
{
    ;
}

void VariableNumberingVisitor::visitPrefixExpression(PrefixExpression *p)
{
    p->right()->accept(this);
}

void VariableNumberingVisitor::visitInfixExpression(InfixExpression *p)
{
    p->lhs()->accept(this);
    p->rhs()->accept(this);
}  

void VariableNumberingVisitor::visitBoolean(Boolean *p)
{
    ;
}

void VariableNumberingVisitor::visitBlockStatement(BlockStatement *p)
{
    for (auto s : p->statements()) {
        s->accept(this);
    }
}

void VariableNumberingVisitor::visitIf(If *p)
{
    p->condition()->accept(this);
    p->consequence()->accept(this);
    p->alternative()->accept(this);
}

void VariableNumberingVisitor::visitFunctionLiteral(FunctionLiteral *p)
{
    params_ = &p->parameters();
    auto b = p->body();
    b->accept(this);
    params_ = nullptr;
}

void VariableNumberingVisitor::visitCallExpression(CallExpression *p)
{
    p->function()->accept(this);
    for (auto arg : p->arguments()) {
        arg->accept(this);
    }
}

void VariableNumberingVisitor::visitStringLiteral(StringLiteral *p)
{
    ;
}

void VariableNumberingVisitor::visitArrayLiteral(ArrayLiteral *p)
{
    for (auto a : p->elements()) {
        a->accept(this);
    }
}

void VariableNumberingVisitor::visitIndexExpression(IndexExpression *p)
{
    p->left()->accept(this);
    p->index()->accept(this);
}

void VariableNumberingVisitor::visitHashLiteral(HashLiteral *p)
{
    for (auto pair : *p) {
        pair.first->accept(this);
        pair.second->accept(this);
    }
}