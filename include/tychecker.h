#pragma once
#include "abstract.h"
#include "symtbl.h"

class tychecker {
private:
    symtbl tbl;
    A_exp *ast;
    tgrTy *check_exp(A_exp *exp);
    tgrTy *check_var(A_var *var);
    void check_dec(A_dec *dec);
    void error(int line, std::string errmsg);
public:
    tychecker(A_exp *ast_) : ast(ast_) {};
    void check();
};