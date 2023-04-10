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
        case expty::VarExp:
            {
                auto e = dynamic_cast<A_VarExp*>(exp);
                return check_var(e->var);
            }
            break;
        case expty::NilExp: return tbl.lookTy("nil"); break;
        case expty::IntExp: return tbl.lookTy("int"); break;
        case expty::StringExp: return tbl.lookTy("string"); break;
        case expty::CallExp:
            {
                auto e = dynamic_cast<A_CallExp*>(exp);
                auto funcTy = tbl.lookFunc(e->func);
                if(funcTy.first == nullptr)
                    error(e->pos, "there is no func named" + e->func);
                auto res = funcTy.first;
                auto list = e->args;
                for(auto argTy : funcTy.second) {
                    // args less than declared
                    if(!list) error(e->pos, "args num mismatch");
                    if(argTy != check_exp(list->head))
                        error(e->pos, "args type mismatch");
                    list = list->tail;
                }
                // args more than declared
                if(list != nullptr) error(e->pos, "args num mismatch");
                return res;
            }
            break;
        case expty::OpExp:
            {
                auto e = dynamic_cast<A_OpExp*>(exp);
                auto l_ty = check_exp(e->left);
                auto r_ty = check_exp(e->right);
                if(l_ty == nullptr || r_ty == nullptr)
                    error(e->pos, "Fail to judge type in left or right or both");
                if(l_ty->ty != TIGTY::INT || r_ty->ty != TIGTY::INT)
                    error(e->pos, "The expression on both sides of the binocular operator must be of type int");
            }
            return tbl.lookTy("int");
            break;
        case expty::RecordExp:
            {
                auto e = dynamic_cast<A_RecordExp*>(exp);
                auto rty = tbl.lookTy(e->type);
                if(!rty || rty->ty != TIGTY::RECORD)
                    error(e->pos, e->type + "is not record type");
                auto &typefields = dynamic_cast<recordTy*>(rty)->fields;
                auto list = e->fields;
                while(list != nullptr) {
                    if(list->head != nullptr) {
                        if(!typefields.count(list->head->name))
                            error(e->pos, e->type + " has no fields named " + list->head->name);
                        if(check_exp(list->head->exp) != tbl.lookTy(list->head->name))
                            error(e->pos, "field and exp type mismatch");
                    }
                    list = list->tail;
                }
                return rty;
            }
            break;
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
        case expty::LetExp:
            {
                auto e = dynamic_cast<A_LetExp*>(exp);
                auto list = e->decs;
                tbl.beginScope();
                while(list) {
                    check_dec(list->head);
                    list = list->tail;
                }
                tbl.endScope();
            }
            break;
        case expty::ForExp:
            {
                auto e = dynamic_cast<A_ForExp*>(exp);
                tbl.beginScope();
                tbl.decVar(e->var, tbl.lookTy("int"));
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
                auto arr_ty = tbl.lookTy(e->type);
                if(arr_ty == nullptr)
                    error(e->pos, "there is no type named " + e->type);
                if(arr_ty->ty != TIGTY::ARRAYTY)
                    error(e->pos, e->type + "is not array type");
                auto arr_ty_ = dynamic_cast<arrayTy*>(arr_ty);
                auto sz_ty = check_exp(e->size);
                if(sz_ty == nullptr || sz_ty->ty != TIGTY::INT)
                    error(e->pos, "Expr in [] are expected to be of type int");
                auto init_ty = check_exp(e->init);
                if(tbl.lookTy(arr_ty_->element_type) != init_ty)
                    error(e->pos, "Initialization expr type mismatch");
                return arr_ty;
            }
            break;
        case expty::BreakExp: return tbl.lookTy("void"); break;
    }
    assert(0);
    return nullptr;
}

tgrTy* tychecker::check_var(A_var *var) {
    assert(var != nullptr);
    using t = A_var::type;
    switch (var->ty)
    {
    case t::SIMPLE:
        {
            auto v = dynamic_cast<A_SimpleVar*>(var);
            return tbl.lookVar(v->sym);
        }
        break;
    case t::FIELD:
        {
            auto v = dynamic_cast<A_FieldVar*>(var);
            auto parTy = check_var(v->var);
            if(!parTy || parTy->ty != TIGTY::RECORD)
                error(v->pos, "parent type not exist or is not record type in field var");
            auto par = dynamic_cast<recordTy*>(parTy);
            if(!par->fields.count(v->sym))
                error(v->pos, "parent type has no field named" + v->sym);
            return par->fields[v->sym];
        }
        break;
    case t::SUBSCRIPT:
        {
            auto v = dynamic_cast<A_SubscriptVar*>(var);
            auto parTy = check_var(v->var);
            if(!parTy || parTy->ty != TIGTY::ARRAYTY)
                error(v->pos, "parent type not exist or is not array type in subscript var");
            auto expTy = check_exp(v->exp);
            if(!expTy || expTy->ty != TIGTY::INT)
                error(v->pos, "Expr in [] are expected to be of type int");
            return tbl.lookTy(dynamic_cast<arrayTy*>(parTy)->element_type);
        }
        break;
    }
    assert(0);
    return nullptr;
}

void tychecker::check_dec(A_dec *dec) {
    using dt = A_dec::type;
    switch (dec->ty)
    {
    case dt::VARD:
        {
            auto d = dynamic_cast<A_VarDec*>(dec);
            // need type deduction
            if(d->type.size() == 0) {
                tbl.decVar(d->var, check_exp(d->init));
            }
            else {
                auto expected_ty = tbl.lookTy(d->type);
                auto init_ty = check_exp(d->init);
                if(!expected_ty || !init_ty || expected_ty != init_ty) {
                    error(d->pos, "types are not exist or mismatch in var declaration");
                }
                tbl.decVar(d->var, expected_ty);
            }
        }
        break;
    case dt::TYDS:
        {
            auto d = dynamic_cast<A_TypeDec*>(dec);
            auto list = d->type;
            for(; list != nullptr; list = list->tail) {
                
                
            }
        }
        break;
    case dt::FUNCDS:
        {
            auto d = dynamic_cast<A_FunctionDec*>(dec);
        }
        break;
    default:
        break;
    }
}