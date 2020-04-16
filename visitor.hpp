#ifndef VISITOR_HPP
#define VISITOR_HPP

//#include "object.hpp"
//#include "ast.hpp"
class Program;
class Object;
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
    protected:
        Program *root_;
};

class Evaluator : public Visitor {
    public:
        explicit Evaluator(Program *root) 
            : Visitor(root), true_o(true), false_o(false) {}
        
        Object *eval() { 
            visitProgram(root_); 
            return ret_;
        }

        void visitIntegerLiteral(IntegerLiteral *a);
        void visitProgram(Program *a);
        void visitExpressionStatement(ExpressionStatement *a);
        void visitBoolean(Boolean *a);
    private:
        void evalStatements(const std::vector<Node *> &statements);
        Object *nativeBoolToObject(bool input);

        Object *ret_ = nullptr;
        Bool true_o; 
        Bool false_o;
        Null null_o;
};

#endif
