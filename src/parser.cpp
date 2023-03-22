#include "parser.h"
#include <iostream>
#include <queue>

token parser::tok() {
    if(q.empty())
        return lex.next();
    auto tmp = q.front();
    q.pop();
    return tmp;
}

token parser::eat(int token_ty) {
    auto t = tok();
    if(t.type != token_ty) {
        token exp(token_ty, -1);
        std::cerr << "in line " << t.line << ": " << std::endl;
        std::cerr << "parser: expected " << exp.to_str() << " but got " << t.to_str() << std::endl;
        exit(1);
    }
    return t;
}

std::unique_ptr<A_dec> parser::dec() {
    auto t = tok();
    switch (t.type)
    {
    case VAR : 
        {
            auto id = eat(IDENTIFIER);
            eat(COLON);
            auto ty = eat(IDENTIFIER);
            auto e = exp();
            return std::unique_ptr<A_dec>(new A_VarDec(t.line, id.val, ty.val, std::move(e)));
        } 
        break;
    default:
        break;
    }
}

std::unique_ptr<A_exp> parser::exp() {
    auto t = tok();
    switch (t.type)
    {
    case LET: 
        {
            auto ds = decs();
            eat(IN);
            auto es = seqexp();
            eat(END);
            return std::unique_ptr<A_exp>(new A_LetExp(t.line, std::move(ds), std::move(es)));
        }
        break;
    case WHILE:
        {
            auto cond = exp();
            eat(DO);
            auto body = exp();
            return std::unique_ptr<A_exp>(new A_WhileExp(t.line, std::move(cond), std::move(body)));
        }
        break;
    case FOR:
        {
            auto ite = eat(IDENTIFIER);
            eat(ASSIGN);
            auto start = exp();
            eat(TO);
            auto end = exp();
            eat(DO);
            auto body = exp();
            return std::unique_ptr<A_exp>(new A_ForExp(t.line, ite.val, 
                                        std::move(start), std::move(end), std::move(body)));
        }
        break;
    case IF:
        {
            auto cond = exp();
            eat(THEN);
            auto True = exp();
            token expexted_else = tok();
            if(expexted_else.type != ELSE) {
                unuse(expexted_else);
                return std::unique_ptr<A_exp>(new A_IfExp(t.line, std::move(cond),
                                            std::move(True), nullptr));
            }
            auto False = exp();
            return std::unique_ptr<A_exp>(new A_IfExp(t.line, std::move(cond),
                                        std::move(True), std::move(False)));
        }
        break;
    case BREAK:
        return std::unique_ptr<A_exp>(new A_BreakExp(t.line));
        break;
    }
    // we can assume that this exp is an exp with operator,
    // and throw it to relevant functions.
    unuse(t);
    return orexp();
}

std::unique_ptr<A_exp> parser::orexp() {
    auto a = andexp();
    auto o = orexp_();
    if(o == nullptr)
        return a;
    std::unique_ptr<A_exp> one_exp(new A_IntExp(a->pos, 1));
    return std::unique_ptr<A_exp>(new A_IfExp(a->pos, std::move(a), std::move(o), std::move(one_exp)));
}

std::unique_ptr<A_exp> parser::orexp_() {
    token expected_or = tok();
    if(expected_or.type != OR) {
        unuse(expected_or);
        return nullptr;
    }
    return orexp();
}

std::unique_ptr<A_exp> parser::andexp() {
    auto r = relexp();
    auto a = andexp_();
    if(a == nullptr)
        return r;
    std::unique_ptr<A_exp> zero_exp(new A_IntExp(a->pos, 0));
    return std::unique_ptr<A_exp>(new A_IfExp(a->pos, std::move(r), std::move(a), std::move(zero_exp)));
}

std::unique_ptr<A_exp> parser::andexp_() {
    token expected_and = tok();
    if(expected_and.type != AND) {
        unuse(expected_and);
        return nullptr;
    }
    return andexp();
}

std::unique_ptr<A_exp> parser::relexp() {
    auto a = addexp();
    int ty = 0;
    auto r = relexp_(ty);
    if(r == nullptr)
        return a;
    A_oper op;
    switch(ty) {
    case EQ: op = A_oper::A_eqOp; break;
    case NEQ: op = A_oper::A_neqOp; break;
    case LE: op = A_oper::A_leOp; break;
    case LT: op = A_oper::A_ltOP; break;
    case GE: op = A_oper::A_geOp; break;
    case GT: op = A_oper::A_gtOp; break;
    }
    return std::unique_ptr<A_exp>(new A_OpExp(a->pos, op, std::move(a), std::move(r)));
}

std::unique_ptr<A_exp> parser::relexp_(int &ty) {
    token t = tok();
    switch (t.type)
    {
    case EQ: case NEQ: case LT:
    case LE: case GT: case GE:
        ty = t.type;
        break;
    default:
        unuse(t);
        return nullptr;
    }
    return relexp();
}

std::unique_ptr<A_exp> parser::addexp() {
    auto m = mulexp();
    int ty = 0;
    auto a = addexp_(ty);
    if(a == nullptr)
        return m;
    A_oper op;
    if(ty == ADD) op = A_oper::A_plusOp;
    else op = A_oper::A_minusOp;
    return std::unique_ptr<A_exp>(new A_OpExp(m->pos, op, std::move(m), std::move(a)));
}

std::unique_ptr<A_exp> parser::addexp_(int &ty) {
    token t = tok();
    if(t.type != ADD && t.type != SUB) {
        unuse(t);
        return nullptr;
    }
    return addexp();
}

std::unique_ptr<A_exp> parser::mulexp() {
    auto s = subexp();
    int ty = 0;
    auto m = mulexp_(ty);
    if(m == nullptr)
        return s;
    A_oper op;
    if(ty == MUL) op = A_oper::A_timesOp;
    else op = A_oper::A_divideOp;
    return std::unique_ptr<A_exp>(new A_OpExp(s->pos, op, std::move(s), std::move(m)));
}

std::unique_ptr<A_exp> parser::mulexp_(int &ty) {
    token t = tok();
    if(t.type != MUL && t.type != DIV) {
        unuse(t);
        return nullptr;
    }
    return mulexp();
}

std::unique_ptr<A_exp> parser::subexp() {
    token t = tok();
    if(t.type != SUB) {
        unuse(t);
        return valexp();
    }
    auto v = valexp();
    std::unique_ptr<A_exp> zero_exp(new A_IntExp(t.line, 0));
    return std::unique_ptr<A_exp>(new A_OpExp(t.line, A_oper::A_minusOp, std::move(zero_exp), std::move(v)));
}

std::unique_ptr<A_exp> parser::valexp() {
    token t = tok();
    switch (t.type)
    {
    case INT_LITERAL:
        return std::unique_ptr<A_exp>(new A_IntExp(t.line, atoi(t.val.c_str())));
    case STR_LITERAL:
        return std::unique_ptr<A_exp>(new A_StringExp(t.line, t.val.substr(1, t.val.length()-2)));
    case NIL:
        return std::unique_ptr<A_exp>(new A_NilExp(t.line));
    default:
        break;
    }
}

std::unique_ptr<A_exp> parser::lval(token &t) {
    switch (t.type)
    {
    case IDENTIFIER: return idexp(t); break;
    case L_SMALL: break; //todo
    default:
        std::cerr << "in line " << t.line << ":" << std::endl;
        std::cerr << "parser: expected left value, but got " << t.to_str() << std::endl;
        exit(1);
    }
}

std::unique_ptr<A_exp> parser::idexp(std::unique_ptr<A_var> var) {
}