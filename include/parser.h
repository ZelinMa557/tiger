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
    std::unique_ptr<A_exp> assignexp();
    std::unique_ptr<A_exp> assignexp_();
    std::unique_ptr<A_exp> orexp();
    std::unique_ptr<A_exp> orexp_();
    std::unique_ptr<A_exp> andexp();
    std::unique_ptr<A_exp> andexp_();
    std::unique_ptr<A_exp> relexp();
    std::unique_ptr<A_exp> relexp_(int &ty);
    std::unique_ptr<A_exp> addexp();
    std::unique_ptr<A_exp> addexp_(int &ty);
    std::unique_ptr<A_exp> mulexp();
    std::unique_ptr<A_exp> mulexp_(int &ty);
    std::unique_ptr<A_exp> subexp();
    std::unique_ptr<A_exp> valexp();
    std::unique_ptr<A_exp> lval();
    std::unique_ptr<A_exp> idexp(std::unique_ptr<A_var> var);
    std::unique_ptr<A_efieldList> efield_list();
    std::unique_ptr<A_efield> efield();
    std::unique_ptr<A_fieldList> field_list();
    std::unique_ptr<A_field> field();
    std::unique_ptr<A_ty> ty();
    std::unique_ptr<A_decList> decs();
    std::unique_ptr<A_exp> seqexp();
public:
    parser() = delete;
    parser(std::string src_file) : lex(src_file) {};
    std::unique_ptr<A_exp> parse();
};