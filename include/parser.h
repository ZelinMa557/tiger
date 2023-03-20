#pragma once
#include <queue>
#include "lexer.h"
#include "abstract.h"

class parser {
private:
    lexer lex;
    std::queue<token> q;
    token eat(int token_ty);
    token tok();
    void unuse(token t) { q.push(t); };
    std::unique_ptr<A_dec> dec();
    std::unique_ptr<A_exp> exp();
    std::unique_ptr<A_exp> elseexp();
    std::unique_ptr<A_decList> decs();
    std::unique_ptr<A_expList> exps();
    std::unique_ptr<A_exp> seqexp();
public:
    parser() = delete;
    parser(std::string src_file) : lex(src_file) {};
    std::unique_ptr<A_exp> parse();
};