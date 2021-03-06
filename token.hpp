#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <memory>
#include "pool.hpp"

#define TOKEN_TYPES               \
    X(T_ILLEGAL, "T_ILLEGAL")     \
    X(T_EOF, "T_EOF")             \
    X(T_IDENT, "T_IDENT")         \
    X(T_INT, "T_INT")             \
    X(T_ASSIGN, "=")              \
    X(T_PLUS, "+")                \
    X(T_COMMA, ",")               \
    X(T_SEMICOLON, "T_SEMICOLON") \
    X(T_LPAREN, "(")              \
    X(T_RPAREN, ")")              \
    X(T_LBRACE, "{")              \
    X(T_RBRACE, "}")              \
    X(T_FUNCTION, "T_FUNCTION")   \
    X(T_LET, "T_LET")             \
    X(T_MINUS, "-")               \
    X(T_BANG, "!")                \
    X(T_ASTERISK, "*")            \
    X(T_SLASH, "/")               \
    X(T_LT, "<")                  \
    X(T_GT, ">")                  \
    X(T_TRUE, "true")             \
    X(T_FALSE, "false")           \
    X(T_IF, "T_IF")               \
    X(T_ELSE, "T_ELSE")           \
    X(T_RETURN, "T_RETURN")       \
    X(T_EQ, "==")                 \
    X(T_NOT_EQ, "!=")             \
    X(T_STRING, "T_STRING")       \
    X(T_LBRACKET, "[")            \
    X(T_RBRACKET, "]")            \
    X(T_COLON, ":")               \
    X(T_MAX, "T_MAX")

#define X(a, b) a,
enum TokenType
{
    TOKEN_TYPES
};
#undef X

#define X(a, b) b,
const char *const tok_names[] = {
    TOKEN_TYPES};
#undef X

class Token;
extern std::unique_ptr<Pool<Token>> TokenPool;

class Token
{
public:
    explicit Token(TokenType type, const char *ch)
        : type_(type), literal_(std::string(ch)) {}

    explicit Token(TokenType type, int ch)
        : type_(type), literal_(std::string(1, ch)) {}

    explicit Token(TokenType type, std::string lit)
        : type_(type), literal_(lit) {}

    const std::string &literal() const { return literal_; }
    TokenType type() const { return type_; }
    bool operator==(const Token &b) const { return type_ == b.type_ && !literal_.compare(b.literal_); }

    template <typename... Args>
    static Token *alloc(Args... args);
    static void dealloc(Token *tok);

private:
    TokenType type_;
    std::string literal_;
};

template <typename... Args>
Token *Token::alloc(Args... args)
{
#ifdef USE_POOL
    if (!TokenPool)
    {
        TokenPool = std::make_unique<Pool<Token>>(30);
    }
    return TokenPool->alloc(std::forward<Args>(args)...);
#else
    return new Token(std::forward<Args>(args)...);
#endif
}

#endif
