#include "parser.h"
#include <iostream>

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
}