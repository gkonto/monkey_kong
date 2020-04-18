#include <sstream>
#include "ast.hpp"
#include "visitor.hpp"

Let::~Let()
{
    if (name_) delete name_;
    if (value_) delete value_;
    //if ()  delete tok_;
}

Identifier::~Identifier()
{
//    delete tok_;
}


PrefixExpression::~PrefixExpression()
{
    if (right_) delete right_;
    //TODO dealloc tok!
    //delete tok_;
}


InfixExpression::~InfixExpression()
{
    delete lhs_;
    //delete tok_;
    delete rhs_;
}

InfixExpression::InfixExpression(Token *tok, Node *lhs, Node *rhs)
     : tok_(tok), lhs_(lhs), op_(tok->literal()), rhs_(rhs)
 {
 }


std::string InfixExpression::asString() const
 {
     std::string ret;

     ret.append("(");
     ret.append(lhs_->asString());
     ret.append(" " + op_ + " ");
     if (rhs_) {
         ret.append(rhs_->asString());
     }
     ret.append(")");

     return ret;
 }


std::string PrefixExpression::asString() const
 {
     std::string ret;

     ret.append("(");
     ret.append(operat_);
     if (right_) {
         ret.append(right_->asString());
     }
     ret.append(")");

     return ret;
 }




Return::~Return()
{
    if (returnValue_) delete returnValue_;
}

std::string Identifier::asString() const
 {
     return value_;
 }

std::string Let::asString() const
 {
     std::string ret;
     ret.append(tokenLiteral());
     ret.append(" ");
     ret.append(name_->asString());
     ret.append(" = ");

     if (value_) { // TODO remove condition in the future
         ret.append(value_->asString());
     }

     ret.append(";");

     return ret;
 }


std::string IntegerLiteral::asString() const
{
    std::stringstream ss;
    ss << value_;
    return ss.str();
}


std::string Return::asString() const
 {
     std::string ret;
     ret.append(tokenLiteral() + " ");

     if (returnValue_) { // TODO remove condition in the future
         ret.append(returnValue_->asString());
     }

     ret.append(";");

     return ret;
 }

std::string ExpressionStatement::asString() const
 {
     if (expression_) { //TODO remove condition in the future
         return expression_->asString();
     }
 
     return "";
 }

std::string Program::asString() const
 {
     std::string ret;
 
     for (auto &a : statements_)
     {
         ret.append(a->asString());
     }
 
     return ret;
 }



BlockStatement::~BlockStatement()
{
    for (auto &stmt : statements_) {
        delete stmt;
    }
}


FunctionLiteral::~FunctionLiteral()
{
     for (auto &param : parameters_) {
         delete param;
     }
     delete body_;
}

std::string If::asString() const
 {
     std::string ret;
     ret.append("if");
     ret.append(condition_->asString());
     ret.append(" ");
     ret.append(consequence_->asString());

     if (alternative_) {
         ret.append("else ");
         ret.append(alternative_->asString());
     }

     return ret;
 }


std::string FunctionLiteral::asString() const
 {
     std::string ret;
     ret.append(tokenLiteral());
     ret.append("(");

     size_t i = 1;
     for (auto &a : parameters_) {
         ret.append(a->asString());
         if (i != parameters_.size()) {
             ret.append(", ");
         }
         i++;
     }
     ret.append(") ");
     ret.append(body_->asString());

     return ret;
 }



 
std::string BlockStatement::asString() const
 {
     std::string ret;
 
     for (auto &a : statements_) {
         ret.append(a->asString());
     }
 
     return ret;
 }




 
 CallExpression::~CallExpression()
 {
     for (auto &arg : arguments_) {
         delete arg;
     }
     delete function_;
 }

std::string CallExpression::asString() const
 {
     std::string ret;
     std::vector<std::string> args;

     ret.append(function_->asString());
     ret.append("(");

     size_t i = 1;
     for (auto &a : arguments_) {
         ret.append(a->asString());
         if (i != arguments_.size()) {
             ret.append(", ");
         }
         i++;
     }
     ret.append(")");

     return ret;
 }

void IntegerLiteral::accept(Visitor &v)
{
    v.visitIntegerLiteral(this);
}


void Program::accept(Visitor &v)
{
    v.visitProgram(this);
}

void ExpressionStatement::accept(Visitor &v)
{
    v.visitExpressionStatement(this);
}


void Boolean::accept(Visitor &v)
{
    v.visitBoolean(this);
}


void PrefixExpression::accept(Visitor &v) {
    v.visitPrefixExpression(this);
}

void InfixExpression::accept(Visitor &v) {
    v.visitInfixExpression(this);
}

void If::accept(Visitor &v) {
    v.visitIfExpression(this);
}

void BlockStatement::accept(Visitor &v) {
    v.visitBlockStatement(this);
}

void Return::accept(Visitor &v) {
    v.visitReturn(this);
}

void Let::accept(Visitor &v) {
    v.visitLet(this);
}


void Identifier::accept(Visitor &v) {
    v.visitIdentifier(this);
}

void FunctionLiteral::accept(Visitor &v) {
    v.visitFunctionLiteral(this);
}

void CallExpression::accept(Visitor &v) {
    v.visitCallExpression(this);
}
