#include "tychecker.h"
#include <assert.h>
void tychecker::error(int line, std::string errmsg) {
    std::cerr << "in line " << line << ":" << std::endl;
    std::cerr << "semantic error: " << errmsg << std::endl;
    exit(1);
}

void tychecker::check() {
    check_exp(ast);
}

tgrTy* tychecker::check_exp(A_exp *exp) {
    if(!exp) return nullptr;
    using expty = A_exp::type;
    switch(exp->ty) {
        case expty::VarExp: break;
        case expty::NilExp: return tbl.lookTy("nil"); break;
        case expty::IntExp: return tbl.lookTy("int"); break;
        case expty::StringExp: return tbl.lookTy("string"); break;
        case expty::CallExp: break;
        case expty::OpExp:
            {
                auto e = dynamic_cast<A_OpExp*>(exp);
                auto l_ty = check_exp(e->left);
                auto r_ty = check_exp(e->right);
                assert(l_ty != nullptr && r_ty != nullptr);
                if(l_ty->ty != TIGTY::INT || r_ty->ty != TIGTY::INT)
                    error(e->pos, "The expression on both sides of the binocular operator must be of type int");
            }
            return tbl.lookTy("int");
            break;
        case expty::RecordExp: break;
        case expty::SeqExp:
            {
                auto list = dynamic_cast<A_SeqExp*>(exp)->seq;
                tgrTy* res = nullptr;
                while(list) {
                    res = check_exp(list->head);
                    list = list->tail;
                }
                return res;
            }
            break;
        case expty::AssignExp:
            {
                auto e = dynamic_cast<A_AssignExp*>(exp);
                auto varTy = check_var(e->var);
                auto expTy = check_exp(e->exp);
                if(varTy != expTy)
                    error(e->pos, "The types at left & right end of the assignment expression do not match");
            }
            return tbl.lookTy("void");
            break;
        case expty::IfExp:
            {
                auto e = dynamic_cast<A_IfExp*>(exp);
                auto condition_ty = check_exp(e->test);
                if(condition_ty == nullptr || condition_ty->ty != TIGTY::INT)
                    error(e->pos, "The test condition for the if statement must be of type int");
                auto then_ty = check_exp(e->then);
                auto else_ty = check_exp(e->elsee);
                if(else_ty == nullptr) return then_ty;
                if(then_ty != else_ty)
                    error(e->pos, "The two branches in the if statement are of inconsistent types");
            }
            break;
        case expty::WhileExp:
            {
                auto e = dynamic_cast<A_WhileExp*>(exp);
                auto con_ty = check_exp(e->test);
                if(con_ty == nullptr || con_ty->ty != TIGTY::INT)
                    error(e->pos, "The test condition for the while statement must be of type int");
                check_exp(e->body);
            }
            return tbl.lookTy("void");
            break;
        case expty::LetExp: break;
        case expty::ForExp:
            {
                auto e = dynamic_cast<A_ForExp*>(exp);
                tbl.beginScope();
                tbl.decVar(e->var, "int");
                auto lo_ty = check_exp(e->lo);
                auto hi_ty = check_exp(e->hi);
                if(lo_ty == nullptr || lo_ty->ty != TIGTY::INT || hi_ty == nullptr || hi_ty->ty != TIGTY::INT)
                    error(e->pos, "The traversal range of the for statement should be of type int");
                check_exp(e->body);
                tbl.endScope();
            }
            return tbl.lookTy("void");
            break;
        case expty::ArrayExp:
            {
                auto e = dynamic_cast<A_ArrayExp*>(exp);
                auto element_ty = tbl.lookTy(e->type);
                if(element_ty == nullptr)
                    error(e->pos, "there is no type named " + e->type);
                auto sz_ty = check_exp(e->size);
                if(sz_ty == nullptr || sz_ty->ty != TIGTY::INT)
                    error(e->pos, "Expr in [] are expected to be of type int");
                auto init_ty = check_exp(e->init);
                if(element_ty != init_ty)
                    error(e->pos, "Initialization expr type mismatch");
            }
            // todo : what is return value?
            break;
        case expty::BreakExp: return tbl.lookTy("void"); break;
    }
    assert(0);
    return nullptr;
}