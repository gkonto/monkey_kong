#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <unordered_map>
#include <functional>

#include "token.hpp"

class Lexer;
class Token;
class Program;
class Node;
class Let;
class Return;

// A Pratt parser. The main idea is the association of parsing functions, 
// which Pratt calls "semantic code" with token types.
class Parser
{
    public:
        /*
         * Whenever a token type is encountered, the parsing functions are
         * called to parse the appropriate expression and return an AST node that represents it. 
         * Each token type can have up to two parsing functions associated with it, depending on whether the
         * token is found in a prefix or an infix position.
         */
        using prefixParseFn = std::function<Node *()>;
        using infixParseFn  = std::function<Node *(Node *)>;

        void registerPrefix(TokenType type, prefixParseFn fun);
        void registerInfix(TokenType type, infixParseFn fun);

        explicit Parser(Lexer *l);
        std::unique_ptr<Program> parseProgram();
    private:
        Let *parseLetStatement();
        Return *parseReturnStatement();
        Node *parseStatement();
        bool curTokenIs(TokenType type) const;
        bool peekTokenIs(TokenType type) const;
        bool expectPeek(TokenType type);

        void nextToken();

        std::unordered_map<TokenType, prefixParseFn> prefixParseFns_;
        std::unordered_map<TokenType, infixParseFn> infixParseFns_;

        Lexer *lexer_      = nullptr;
        Token *cur_token_  = nullptr;
        Token *next_token_ = nullptr;
};

#endif
