#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>

#include "token.hpp"

class Token;

struct Node
{
    virtual ~Node() {}
    virtual const char *tokenLiteral() const = 0; // Only for debugging and testing
};

// Program is the root node of every AST
// our parser produces.
class Program : public Node
{
    public:
        ~Program();
        const char *tokenLiteral() const { return "Program node"; };
        size_t size() const { return statements_.size(); }
        void emplace_back(Node *stmt) { statements_.emplace_back(); };
        Node *operator[](std::size_t idx) const { return statements_[idx]; }
        const std::vector<Node *> &statements() const { return statements_; }
    private:
        std::vector<Node *> statements_;
};


class Identifier : public Node
{
    public:
        explicit Identifier(Token *tok, const std::string &value);
        ~Identifier();
        const char *tokenLiteral() const { return tok_->literal(); }
        const std::string &value() const { return value_; }
    private:
        Token *tok_;
        std::string value_;
};


class Let : public Node
{
    public:
        explicit Let(Token *tok, Identifier *p_name, Node *p_value)
            : tok_(tok), name_(p_name), value_(p_value) {}

        ~Let();
        const char *tokenLiteral() const { return tok_->literal(); }
        void setName(Identifier *name) { name_ = name; }
        Identifier *name() const { return name_; }
        Node *value() const { return value_; }
        void setValue(Node *exp) { value_ = exp; }
    private:
        Token *tok_;
        Identifier *name_;   // the identifier's name
        Node *value_; // the expression that produces a value
};



#endif
