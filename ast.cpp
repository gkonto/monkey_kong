#include "ast.hpp"


Let::~Let()
{
    if (name_) delete name_;
    if (value_) delete value_;
//    delete tok_;
}

Identifier::~Identifier()
{
//    delete tok_;
}


Return::~Return()
{
    if (returnValue_) delete returnValue_;
}



