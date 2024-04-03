#pragma once
#include <queue>
#include <memory>
#include <tuple>
#include <optional>
#include "lexer/lexer.h"
#include "parser/ast.h"
namespace tiger {
class parser {
private:
    std::unique_ptr<lexer> lex;
    std::queue<token> q;
    token eat(int token_ty);
    token tok();
    void unuse(token t) { q.push(t); };
    ptr<A_dec> dec();
    ptr<A_exp> exp();
    ptr<A_exp> binary_exp(int min_binding_power);
    // item can be: var_exp, int or string literal, call_exp, seqexp, nil, array exp, record exp
    ptr<A_exp> item();
    std::optional<std::tuple<A_oper, int, int>> get_operator(const token &tok); 
    std::vector<ptr<A_efield>> efield_list();
    ptr<A_efield> efield();
    std::vector<ptr<A_field>> field_list();
    ptr<A_field> field();
    ptr<A_ty> ty();
    std::vector<ptr<A_dec>> decs();
    ptr<A_exp> seqexp();
public:
    parser() = delete;
    parser(std::unique_ptr<lexer> lex_) : lex(std::move(lex_)) {};
    ptr<A_exp> parse();
};
}