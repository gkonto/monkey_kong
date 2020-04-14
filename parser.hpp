#ifndef PARSER_HPP
#define PARSER_HPP


class Lexer;
class Token;

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
        explicit Parser(Lexer *l);
    private:
        void nextToken();
        Lexer *lexer_      = nullptr;
        Token *cur_token_  = nullptr;
        Token *next_token_ = nullptr;
};

#endif
