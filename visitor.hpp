#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <vector>

class Program;
class Identifier;
class Let;
class Return;
class ExpressionStatement;
class IntegerLiteral;
class PrefixExpression;
class InfixExpression;
class Boolean;
class BlockStatement;
class If;
class FunctionLiteral;
class CallExpression;
class StringLiteral;
class ArrayLiteral;
class IndexExpression;
class HashLiteral;

class Visitor {
    public:
        virtual void visitProgram(Program *p) = 0;
        virtual void visitIdentifier(Identifier *p) = 0;
        virtual void visitLet(Let *p) = 0;
        virtual void visitReturn(Return *p) = 0;
        virtual void visitExpressionStatement(ExpressionStatement *p) = 0;
        virtual void visitIntegerLiteral(IntegerLiteral *p) = 0;
        virtual void visitPrefixExpression(PrefixExpression *p) = 0;
        virtual void visitInfixExpression(InfixExpression *p) = 0;
        virtual void visitBoolean(Boolean *p) = 0;
        virtual void visitBlockStatement(BlockStatement *p) = 0;
        virtual void visitIf(If *p) = 0;
        virtual void visitFunctionLiteral(FunctionLiteral *p) = 0;
        virtual void visitCallExpression(CallExpression *p) = 0;
        virtual void visitStringLiteral(StringLiteral *p) = 0;
        virtual void visitArrayLiteral(ArrayLiteral *p) = 0;
        virtual void visitIndexExpression(IndexExpression *p) = 0;
        virtual void visitHashLiteral(HashLiteral *p) = 0;
};

class VariableNumberingVisitor : public Visitor
{
    public:
        void visitProgram(Program *p);
        void visitIdentifier(Identifier *p);
        void visitLet(Let *p);
        void visitReturn(Return *p);
        void visitExpressionStatement(ExpressionStatement *p);
        void visitIntegerLiteral(IntegerLiteral *p);
        void visitPrefixExpression(PrefixExpression *p);
        void visitInfixExpression(InfixExpression *p);
        void visitBoolean(Boolean *p);
        void visitBlockStatement(BlockStatement *p);
        void visitIf(If *p);
        void visitFunctionLiteral(FunctionLiteral *p);
        void visitCallExpression(CallExpression *p);
        void visitStringLiteral(StringLiteral *p);
        void visitArrayLiteral(ArrayLiteral *p);
        void visitIndexExpression(IndexExpression *p);
        void visitHashLiteral(HashLiteral *p);        
    private:
        const std::vector<Identifier *> *params_ = nullptr;
};



#endif