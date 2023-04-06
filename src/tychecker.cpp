#include "tychecker.h"

void tychecker::check() {
    check_exp(ast);
}

tgrTy* tychecker::check_exp(A_exp *exp) {
    using expty = A_exp::type;
    switch(exp->ty) {
        case expty::VarExp: break;
        case expty::NilExp: break;
        case expty::IntExp: break;
        case expty::StringExp: break;
        case expty::CallExp: break;
        case expty::OpExp: break;
        case expty::RecordExp: break;
        case expty::SeqExp: break;
        case expty::AssignExp: break;
        case expty::IfExp: break;
        case expty::WhileExp: break;
        case expty::LetExp: break;
        case expty::ForExp: break;
        case expty::ArrayExp: break;
        case expty::BreakExp: break;
    }
}