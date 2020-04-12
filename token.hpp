#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

#define TOKEN_TYPES\
    X(T_ILLEGAL, "T_ILLEGAL")\
    X(T_EOF, "T_EOF")\
    X(T_IDENT, "T_IDENT")\
    X(T_INT, "T_INT")\
    X(T_ASSIGN, "=")\
    X(T_PLUS, "+")\
    X(T_COMMA, "T_COMMA")\
    X(T_SEMICOLON, "T_SEMICOLON")\
    X(T_LPAREN, "(")\
    X(T_RPAREN, ")")\
    X(T_LBRACE, "{")\
    X(T_RBRACE, "}")\
    X(T_FUNCTION, "T_FUNCTION")\
    X(T_LET, "T_LET")\
    X(T_MAX, "T_MAX")

#define X(a, b) a,
enum TokenType
{
    TOKEN_TYPES
};
#undef X

#define X(a, b) b,
const char *const tok_names[] = {
  TOKEN_TYPES
};
#undef X


class Token
{
    public:
        explicit Token(TokenType type, const char *ch);
    private:
        const char *literal_;
        TokenType type_;
};

#endif
