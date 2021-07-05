#ifndef VISITOR_HPP
#define VISITOR_HPP

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
        virtual void visitProgram(Program *p) const = 0;
        virtual void visitIdentifier(Identifier *p) const = 0;
        virtual void visitLet(Let *p) const = 0;
        virtual void visitReturn(Return *p) const = 0;
        virtual void visitExpressionStatement(ExpressionStatement *p) const = 0;
        virtual void visitIntegerLiteral(IntegerLiteral *p) const = 0;
        virtual void visitPrefixExpression(PrefixExpression *p) const = 0;
        virtual void visitInfixExpression(InfixExpression *p) const = 0;
        virtual void visitBoolean(Boolean *p) const = 0;
        virtual void visitBlockStatement(BlockStatement *p) const = 0;
        virtual void visitIf(If *p) const = 0;
        virtual void visitFunctionLiteral(FunctionLiteral *p) const = 0;
        virtual void visitCallExpression(CallExpression *p) const = 0;
        virtual void visitStringLiteral(StringLiteral *p) const = 0;
        virtual void visitArrayLiteral(ArrayLiteral *p) const = 0;
        virtual void visitIndexExpression(IndexExpression *p) const = 0;
        virtual void visitHashLiteral(HashLiteral *p) const = 0;
};



#endif