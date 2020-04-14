#include "ast.hpp"


Let::~Let()
{
    delete tok_;
}

Identifier::~Identifier()
{
    delete tok_;
}



