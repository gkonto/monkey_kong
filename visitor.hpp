#ifndef VISITOR_HPP
#define VISITOR_HPP

class Single;

#include "object.hpp"
#include "ast.hpp"

class Visitor {
    public:
        explicit Visitor(Program *root) 
            : root_(root) {}

        virtual void visitIntegerLiteral(IntegerLiteral *a) = 0;
        virtual void visitProgram(Program *a) = 0;
        virtual void visitExpressionStatement(ExpressionStatement *a) = 0;
        virtual void visitBoolean(Boolean *a) = 0;
        virtual void visitPrefixExpression(PrefixExpression *a) = 0;
        virtual void visitInfixExpression(InfixExpression *a) = 0;
        virtual void visitBlockStatement(BlockStatement *a) = 0;
        virtual void visitIfExpression(If *a) = 0;
        virtual void visitReturn(Return *a) = 0;
    protected:
        Program *root_;
};

class Evaluator : public Visitor {
    public:
        explicit Evaluator(Program *root) 
            : Visitor(root) {}
        
        Single *eval() { 
            visitProgram(root_); 
            return ret_;
        }

        void visitIntegerLiteral(IntegerLiteral *a);
        void visitProgram(Program *a);
        void visitExpressionStatement(ExpressionStatement *a);
        void visitBoolean(Boolean *a);
        void visitPrefixExpression(PrefixExpression *a);
        void visitInfixExpression(InfixExpression *a);
        void visitBlockStatement(BlockStatement *a);
        void visitIfExpression(If *a);
        void visitReturn(Return *a);
    private:
        void evalStatements(const std::vector<Node *> &statements);
        void evalPrefixExpression(const std::string &op);
        void evalBangOperatorExpression();
        void evalMinusPrefixOperatorExpression();
        void evalInfixExpression(const std::string &op, Single *left, Single *right);
        void evalIntegerInfixExpression(const std::string &op, Single *left, Single *right);
        void evalProgram(Program *a);
        void evalBlockStatement(BlockStatement *a);
        bool isError(Single *val) const;
        Single *nativeBoolToSingObj(bool input);
        bool isTruthy(Single *obj) const;

        void setResult(Single *new_obj);
        Single *setResultNull();
        void conditionalDelete(Single *del_ent);

        Single *ret_ = nullptr; // TODO encapsulate so  that cannot be modified without dellocation
};

#endif
