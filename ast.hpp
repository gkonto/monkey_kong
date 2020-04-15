#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>

#include "token.hpp"

class Token;

struct Node
{
    virtual ~Node() {}
    virtual const std::string &tokenLiteral() const { return literal; }; // Only for debugging and testing
    virtual std::string asString() const = 0;
    std::string literal = "No token node";
};

// Program is the root node of every AST
// our parser produces.
class Program : public Node
{
    public:
        ~Program() 
        {
            for (auto &stmt : statements_) {
                delete stmt;
            }
        }
        std::string asString() const;
        size_t size() const { return statements_.size(); }
        void emplace_back(Node *stmt) { statements_.emplace_back(stmt); };
        Node *operator[](std::size_t idx) const { return statements_[idx]; }
        const std::vector<Node *> &statements() const { return statements_; }
        std::vector<Node *>::iterator begin() { return statements_.begin(); }
        std::vector<Node *>::iterator end() { return statements_.end(); }
    private:
        std::vector<Node *> statements_;
};


class Identifier : public Node
{
    public:
        explicit Identifier(Token *tok, const std::string &value)
            : tok_(tok), value_(value) {}

        ~Identifier();
        std::string asString() const;
        const std::string &tokenLiteral() const { return tok_->literal(); }
        const std::string &value() const { return value_; }
    private:
        Token *tok_;
        std::string value_;
};


class Let : public Node
{
    public:
        explicit Let(Token *tok, Identifier *p_name = nullptr, Node *p_value = nullptr)
            : tok_(tok), name_(p_name), value_(p_value) {}

        ~Let();
        std::string asString() const;
        const std::string &tokenLiteral() const { return tok_->literal(); }
        void setName(Identifier *name) { name_ = name; }
        Identifier *name() const { return name_; }
        const std::string &identName() const { return name_->value(); }
        Node *value() const { return value_; }
        void setValue(Node *exp) { value_ = exp; }
    private:
        Token *tok_;
        Identifier *name_;   // the identifier's name
        Node *value_; // the expression that produces a value
};


class Return : public Node
{
    public:
        explicit Return(Token *tok, Node *exp) :
            token_(tok), returnValue_(exp) {}
        ~Return();

        std::string asString() const;
        const std::string &tokenLiteral() const { return token_->literal(); }
        void setReturnVal(Node *exp) { returnValue_ = exp; }
        Node *value() const { return returnValue_; }
    private:
        Token *token_; // The return statement
        Node *returnValue_;
};


class ExpressionStatement : public Node
 {
     public:
         ~ExpressionStatement()
         {
             if (expression_) delete expression_;
         };

        std::string asString() const;
         Node *expression() const { return expression_; }
         void setExpression(Node *exp) { expression_ = exp; }
     private:
         Node *expression_;
 };


 class IntegerLiteral : public Node
 {
     public:
         IntegerLiteral(Token *tok, int value) 
             : token_(tok), value_(value) {}

         const std::string &tokenLiteral() const { return token_->literal(); }
         int value() const { return value_; }
         std::string asString() const;
     private:
         Token *token_ = nullptr;
         int value_ = 0;
 };



#endif
