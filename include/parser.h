#pragma once
#include <queue>
#include "lexer.h"
#include "abstract.h"
namespace FRONTEND {
class parser {
private:
    lexer lex;
    std::queue<token> q;
    token eat(int token_ty);
    token tok();
    void unuse(token t) { q.push(t); };
    A_dec* dec();
    A_exp* exp();
    A_exp* assignexp();
    A_exp* assignexp_();
    A_exp* orexp();
    A_exp* orexp_(A_exp *parent);
    A_exp* andexp();
    A_exp* andexp_(A_exp *parent);
    A_exp* relexp();
    A_exp* relexp_(A_exp *parent);
    // A_exp* relexp_(int &ty);
    A_exp* addexp();
    A_exp* addexp_(A_exp *parent);
    A_exp* mulexp();
    A_exp* mulexp_(A_exp *parent);
    A_exp* subexp();
    A_exp* valexp();
    A_exp* lval();
    A_exp* idexp(A_var* var);
    A_efieldList* efield_list();
    A_efield* efield();
    A_fieldList* field_list();
    A_field* field();
    A_ty* ty();
    A_decList* decs();
    A_exp* seqexp();
public:
    parser() = delete;
    parser(std::string src_file) : lex(src_file) {};
    A_exp* parse();
};
}