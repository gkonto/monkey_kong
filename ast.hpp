#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <map>

#include "token.hpp"
#include "object.hpp"

class Token;
class Visitor;

struct Node {
  virtual ~Node() {}
  virtual const std::string &tokenLiteral() const { return literal; } // Only for debugging and testing
  virtual std::string asString() const = 0;
  virtual Single *eval(Environment *s) = 0;
  std::string literal = "No token node";
};


// Program is the root node of every AST
// our parser produces.
class Program : public Node
{
    public:
        ~Program() {
            for (auto &stmt : statements_) {
                delete stmt;
            }
            TokenPool.reset();
        }
        std::string asString() const;
        size_t size() const { return statements_.size(); }
        void emplace_back(Node *stmt) { statements_.emplace_back(stmt); };
        Node *operator[](std::size_t idx) const { return statements_[idx]; }
        const std::vector<Node *> &statements() const { return statements_; }
        std::vector<Node *>::iterator begin() { return statements_.begin(); }
        std::vector<Node *>::iterator end() { return statements_.end(); }
        Single *eval(Environment *s);
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
        Single *eval(Environment *s);
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
        Single *eval(Environment *s);
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
        Single *eval(Environment *s);
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
        Single *eval(Environment *s);
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
        Single *eval(Environment *s);
     private:
         Token *token_ = nullptr;
         int value_ = 0;
 };



 class PrefixExpression : public Node
 {
     public:
         PrefixExpression(Token *tok) :
             operat_(tok->literal()), tok_(tok) {}
         ~PrefixExpression();

         const std::string &tokenLiteral() const { return tok_->literal(); }
         std::string asString() const;
         const std::string &operator_s() const { return operat_; }
         Node *right() const { return right_; }
         void setRight(Node *exp) { right_ = exp; }
        Single *eval(Environment *s);
     private:
         std::string operat_;
         Token *tok_; // The prefix token (eg !)
         Node *right_;
 };


 class InfixExpression : public Node
 {
     public:
         InfixExpression(Token *tok, Node *lhs, Node *rhs = nullptr);
         ~InfixExpression();
         const std::string &tokenLiteral() const { return tok_->literal(); }
         std::string asString() const;
         Node *lhs() const { return lhs_; }
         void setRhs(Node *rhs) { rhs_ = rhs; }
         Node *rhs() const { return rhs_; }
         TokenType op() const { return op_; }
        Single *eval(Environment *s);
     private:
         Token *tok_; // The operator token, e.g +
         Node *lhs_;
         TokenType op_;
         Node *rhs_;
 };


 class Boolean : public Node
 {
     public:
         explicit Boolean(Token *tok, bool value)
            : tok_(tok), value_(value) {}
         const std::string &tokenLiteral() const { return tok_->literal(); }
         bool value() const { return value_; }
         std::string asString() const { return tok_->literal(); }
        Single *eval(Environment *s);
     private:
         Token *tok_;
         bool value_;
 };
 
 class BlockStatement : public Node
 {
     public:
         explicit BlockStatement(Token *tok) : tok_(tok) {}
         ~BlockStatement();
         std::string asString() const;
         const std::string &tokenLiteral() const { return tok_->literal(); }
         size_t size() const { return statements_.size(); }
         Node *operator[](std::size_t idx) const { return statements_[idx]; }
         void emplace_back(Node *stmt) { return statements_.emplace_back(stmt); }
         const std::vector<Node *> &statements() const { return statements_; }
        Single *eval(Environment *s);
     private:
         Token *tok_;
         std::vector<Node *> statements_;
 };


 class If : public Node
 {
     public:
         explicit If(Token *tok) : tok_(tok) {}
         ~If() {
             if (consequence_) delete consequence_;
             if (alternative_) delete alternative_;
             if (condition_) delete condition_;
         }
 
         const std::string &tokenLiteral() const { return tok_->literal(); }
         std::string asString() const;
         Node *condition() const { return condition_; }
         void setCondition(Node *condition) { condition_ = condition; }
         BlockStatement *consequence() const { return consequence_; }
         void setConsequence(BlockStatement *consequence) { consequence_ = consequence; }
         BlockStatement *alternative() const { return alternative_; }
         void setAlternative(BlockStatement *alternative) { alternative_ = alternative; }
        Single *eval(Environment *s);

     private:
         Token *tok_ = nullptr;
         Node *condition_ = nullptr;
         BlockStatement *consequence_ = nullptr;
         BlockStatement *alternative_ = nullptr;
 };

 
 class FunctionLiteral : public Node
 {
     public:
         explicit FunctionLiteral(Token *tok) : tok_(tok) {}
         ~FunctionLiteral();
         std::string asString() const;
         const std::string &tokenLiteral() const { return tok_->literal(); }
         Identifier *param(std::size_t idx) const { return parameters_[idx]; }
         void setBody(BlockStatement *body) { body_ = body; }
         BlockStatement *body() const { return body_; }
         void setParameters(const std::vector<Identifier *> &parameters) { parameters_ = parameters; }
         std::vector<Identifier *> &parameters() { return parameters_; }
         size_t paramSize() const { return parameters_.size(); }
        Single *eval(Environment *s);
     private:
         Token *tok_;
         std::vector<Identifier *> parameters_; //used by object if created
         BlockStatement *body_ = nullptr;//used by object if created
 };
 

 class CallExpression : public Node
 {
     public:
         explicit CallExpression(Token *tok, Node *fun) : tok_(tok), function_(fun) {}
         ~CallExpression();
         std::string asString() const;
         const std::string &tokenLiteral() const { return tok_->literal(); }
         size_t size() const { return arguments_.size(); }
         Node *function() const { return function_; }
         Node *argNum(std::size_t idx) const { return arguments_[idx]; }
         void setArguments(std::vector<Node *> args) { arguments_ = args; }
         const std::vector<Node *> &arguments() const { return arguments_; }
        Single *eval(Environment *s);
     private:
         Token *tok_;
         Node *function_;
         std::vector<Node *> arguments_;
 };


class StringLiteral : public Node
{
    public: 
        explicit StringLiteral(Token *tok) 
            : value_(tok->literal()), tok_(tok) {}

        std::string asString() const;
        const std::string &value() const { return value_; }
        Single *eval(Environment *s) { return Single::alloc(value_.c_str(), STRING); }
    private:
        std::string value_;
        Token *tok_;
};


class ArrayLiteral : public Node
{
    public:
        ArrayLiteral(Token *tok, std::vector<Node *> &elements)
            : elements_(elements), tok_(tok) {}

        ~ArrayLiteral();
        std::string asString() const;
        size_t size() const { return elements_.size(); }
        Node *at(size_t idx) const { return elements_.at(idx); }
        const std::vector<Node *> &elements() const { return elements_; }
        Single *eval(Environment *s);
    private:
        std::vector<Node *>elements_;
        Token *tok_;
};


class IndexExpression : public Node
{
    public:
        IndexExpression(Token *tok, Node *left, Node *index = nullptr) 
            : tok_(tok), left_(left), index_(index) {

        }
        ~IndexExpression() {
            delete left_;
            if (index_) {
                delete index_;
            }
        }
        std::string asString() const;
        Node *index() const { return index_; }
        Node *left() const { return left_; }
        void setIndex(Node *index) { index_ = index; }
        Single *eval(Environment *s);
    private:
        Token *tok_;
        Node *left_;
        Node *index_;
};


class HashLiteral : public Node
{
    public:
        using HashMap = std::map<Node *, Node *>;

        explicit HashLiteral(Token *tok) : tok_(tok) {}
        ~HashLiteral() {
            for (auto &a : pairs_) {
                delete a.first;
                delete a.second;
            }
        }

        std::string asString() const;
        const std::string &tokenLiteral() const { return tok_->literal(); }
        size_t size() const { return pairs_.size(); }
        HashMap::const_iterator begin() const { return pairs_.begin(); }
        HashMap::const_iterator end() const { return pairs_.end(); }
        void emplace(Node *key, Node *value) {  pairs_.emplace(key, value); }
        Single *eval(Environment *s);
    private:
        Token *tok_;
        HashMap pairs_;
};


#endif
