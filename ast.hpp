#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>

#include "token.hpp"
#include "object.hpp"

//#define USEVISITOR
//#define NODISPATCH

class Token;
class Visitor;

enum AstNodeType {
    AST_PROGRAM,
    AST_IDENTIFIER,
    AST_LET,
    AST_RETURN,
    AST_EXPRESSIONSTATEMENT,
    AST_INTEGERLITERAL,
    AST_INFIXEXPRESSION,
    AST_ARRAYLITERAL,
    AST_PREFIXEXPRESSION,
    AST_BOOLEAN,
    AST_BLOCKSTATEMENT,
    AST_INDEXEXPRESSION,
    AST_IF,
    AST_FUNCTIONLITERAL,
    AST_CALLEXPRESSION,
    AST_STRINGLITERAL
};

struct Node
{
    Node(AstNodeType type) : type_(type) {}
    virtual ~Node() {}
    virtual const std::string &tokenLiteral() const { return literal; }; // Only for debugging and testing
    virtual std::string asString() const = 0;
    virtual void accept(Visitor &) = 0;
#ifdef NODISPATCH
    Single *eval(Environment *s);
#else
    virtual Single *eval(Environment *s) = 0;
#endif
    std::string literal = "No token node";
    AstNodeType type_;    
};

// Program is the root node of every AST
// our parser produces.
class Program : public Node
{
    public:
        Program() : Node(AST_PROGRAM) {}
        ~Program() 
        {
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
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalProgram(Environment *s);
    private:
        std::vector<Node *> statements_;
};


class Identifier : public Node
{
    public:
        explicit Identifier(Token *tok, const std::string &value)
            : Node(AST_IDENTIFIER), tok_(tok), value_(value) {}

        ~Identifier();
        std::string asString() const;
        const std::string &tokenLiteral() const { return tok_->literal(); }
        const std::string &value() const { return value_; }
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalIdentifier(Environment *s);
    private:
        Token *tok_;
        std::string value_;
};


class Let : public Node
{
    public:
        explicit Let(Token *tok, Identifier *p_name = nullptr, Node *p_value = nullptr)
            : Node(AST_LET), tok_(tok), name_(p_name), value_(p_value) {}

        ~Let();
        std::string asString() const;
        const std::string &tokenLiteral() const { return tok_->literal(); }
        void setName(Identifier *name) { name_ = name; }
        Identifier *name() const { return name_; }
        const std::string &identName() const { return name_->value(); }
        Node *value() const { return value_; }
        void setValue(Node *exp) { value_ = exp; }
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalLet(Environment *s);
    private:
        Token *tok_;
        Identifier *name_;   // the identifier's name
        Node *value_; // the expression that produces a value
};


class Return : public Node
{
    public:
        explicit Return(Token *tok, Node *exp) :
            Node(AST_RETURN), token_(tok), returnValue_(exp) {}
        ~Return();

        std::string asString() const;
        const std::string &tokenLiteral() const { return token_->literal(); }
        void setReturnVal(Node *exp) { returnValue_ = exp; }
        Node *value() const { return returnValue_; }
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalReturn(Environment *s);
    private:
        Token *token_; // The return statement
        Node *returnValue_;
};


class ExpressionStatement : public Node
 {
     public:
         ExpressionStatement() : Node(AST_EXPRESSIONSTATEMENT) {}
         ~ExpressionStatement()
         {
             if (expression_) delete expression_;
         };

        std::string asString() const;
        Node *expression() const { return expression_; }
        void setExpression(Node *exp) { expression_ = exp; }
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalExpressionStatement(Environment *s);
     private:
         Node *expression_;
 };


 class IntegerLiteral : public Node
 {
     public:
         IntegerLiteral(Token *tok, int value) 
             : Node(AST_INTEGERLITERAL), token_(tok), value_(value) {}

         const std::string &tokenLiteral() const { return token_->literal(); }
         int value() const { return value_; }
         std::string asString() const;
         virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalIntegerLiteral(Environment *s);
     private:
         Token *token_ = nullptr;
         int value_ = 0;
 };



 class PrefixExpression : public Node
 {
     public:
         PrefixExpression(Token *tok) :
             Node(AST_PREFIXEXPRESSION), operat_(tok->literal()), tok_(tok) {}
         ~PrefixExpression();

         const std::string &tokenLiteral() const { return tok_->literal(); }
         std::string asString() const;
         const std::string &operator_s() const { return operat_; }
         Node *right() const { return right_; }
         void setRight(Node *exp) { right_ = exp; }
         virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalPrefixExpression(Environment *s);
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
         virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalInfixExpression(Environment *s);
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
            : Node(AST_BOOLEAN), tok_(tok), value_(value) {}
         const std::string &tokenLiteral() const { return tok_->literal(); }
         bool value() const { return value_; }
         std::string asString() const { return tok_->literal(); }
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalBoolean(Environment *s);
     private:
         Token *tok_;
         bool value_;
 };
 
 class BlockStatement : public Node
 {
     public:
         explicit BlockStatement(Token *tok) : Node(AST_BLOCKSTATEMENT), tok_(tok) {}
         ~BlockStatement();
         std::string asString() const;
         const std::string &tokenLiteral() const { return tok_->literal(); }
         size_t size() const { return statements_.size(); }
         Node *operator[](std::size_t idx) const { return statements_[idx]; }
         void emplace_back(Node *stmt) { return statements_.emplace_back(stmt); }
         const std::vector<Node *> &statements() const { return statements_; }
         virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalBlockStatement(Environment *s);
     private:
         Token *tok_;
         std::vector<Node *> statements_;
 };


 class If : public Node
 {
     public:
         explicit If(Token *tok) : Node(AST_IF), tok_(tok) {}
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
         virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalIf(Environment *s);
     private:
         Token *tok_ = nullptr;
         Node *condition_ = nullptr;
         BlockStatement *consequence_ = nullptr;
         BlockStatement *alternative_ = nullptr;
 };

 
 class FunctionLiteral : public Node
 {
     public:
         explicit FunctionLiteral(Token *tok) : Node(AST_FUNCTIONLITERAL), tok_(tok) {}
         ~FunctionLiteral();
         std::string asString() const;
         const std::string &tokenLiteral() const { return tok_->literal(); }
         Identifier *param(std::size_t idx) const { return parameters_[idx]; }
         void setBody(BlockStatement *body) { body_ = body; }
         BlockStatement *body() const { return body_; }
         void setParameters(const std::vector<Identifier *> &parameters) { parameters_ = parameters; }
         std::vector<Identifier *> &parameters() { return parameters_; }
         size_t paramSize() const { return parameters_.size(); }
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalFunctionLiteral(Environment *s);
     private:
         Token *tok_;
         std::vector<Identifier *> parameters_; //used by object if created
         BlockStatement *body_ = nullptr;//used by object if created
 };
 

 class CallExpression : public Node
 {
     public:
         explicit CallExpression(Token *tok, Node *fun) : Node(AST_CALLEXPRESSION), tok_(tok), function_(fun) {}
         ~CallExpression();
         std::string asString() const;
         const std::string &tokenLiteral() const { return tok_->literal(); }
         size_t size() const { return arguments_.size(); }
         Node *function() const { return function_; }
         Node *argNum(std::size_t idx) const { return arguments_[idx]; }
         void setArguments(std::vector<Node *> args) { arguments_ = args; }
         const std::vector<Node *> &arguments() const { return arguments_; }
        virtual void accept(Visitor &v);
        Single *eval(Environment *s);
        Single *evalCallExpression(Environment *s);
     private:
         Token *tok_;
         Node *function_;
         std::vector<Node *> arguments_;
 };


class StringLiteral : public Node
{
    public: 
        explicit StringLiteral(Token *tok) 
            : Node(AST_STRINGLITERAL), value_(tok->literal()), tok_(tok) {}

        std::string asString() const;
        const std::string &value() const { return value_; }
        Single *eval(Environment *s) { return Single::alloc(value_.c_str(), STRING); }
        void accept(Visitor &v) {};
    private:
        std::string value_;
        Token *tok_;
};


class ArrayLiteral : public Node
{
    public:
        ArrayLiteral(Token *tok, std::vector<Node *> &elements)
            : Node(AST_ARRAYLITERAL), elements_(elements), tok_(tok) {}

        ~ArrayLiteral();
        std::string asString() const;
        size_t size() const { return elements_.size(); }
        Node *at(size_t idx) const { return elements_.at(idx); }
        const std::vector<Node *> &elements() const { return elements_; }
        Single *eval(Environment *s);
        void accept(Visitor &v) { 
            std::cout << "ArrayLiteral::accept(): TODO" << std::endl;
            exit(1);

        }
    private:
        std::vector<Node *>elements_;
        Token *tok_;
};


class IndexExpression : public Node
{
    public:
        IndexExpression(Token *tok, Node *left, Node *index = nullptr) 
            : Node(AST_INDEXEXPRESSION), tok_(tok), left_(left), index_(index) {

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
        Single *eval(Environment *s) { return nullptr; }
        void accept(Visitor &v) {}
    private:
        Token *tok_;
        Node *left_;
        Node *index_;
};


#endif
