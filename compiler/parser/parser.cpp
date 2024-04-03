#include "parser/parser.h"
#include <iostream>
#include <queue>
#include <vector>
#include <assert.h>
namespace tiger {
namespace {
ptr<A_exp> binding_expr_oper(A_pos pos, A_oper op, ptr<A_exp> lhs, ptr<A_exp> rhs) {
    switch (op)
    {
    case A_oper::A_assignOp:
        {
            if (lhs->ty != A_exp::type::VarExp) {
                std::cerr << "in line " << pos << ", only variable can be assigned" << std::endl;
                exit(1);
            }
            auto var_exp = dynamic_cast<A_VarExp*>(lhs.get());
            return make_shared<A_AssignExp>(pos, var_exp->var, rhs);
        }
    case A_oper::A_andOp:
        {
            auto zero = make_shared<A_IntExp>(pos, 0);
            return make_shared<A_IfExp>(pos, lhs, rhs, zero);
        }
    case A_oper::A_orOP:
        {
            auto one = make_shared<A_IntExp>(pos, 1);
            return make_shared<A_IfExp>(pos, lhs, one, rhs);
        }
    }
    return make_shared<A_OpExp>(pos, op, lhs, rhs);
}
}
token parser::tok() {
    if(q.empty())
        return lex->next();
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

std::optional<std::tuple<A_oper, int, int>> parser::get_operator(const token &tok) {
    static std::unordered_map<int, std::tuple<A_oper, int, int>> result {
        {ASSIGN, {A_oper::A_assignOp, 2, 1}},

        {AND, {A_oper::A_andOp, 3, 4}},
        {OR, {A_oper::A_orOP, 3, 4}},

        {LE, {A_oper::A_leOp, 5, 6}},
        {LT, {A_oper::A_ltOP, 5, 6}},
        {GE, {A_oper::A_geOp, 5, 6}},
        {GT, {A_oper::A_gtOp, 5, 6}},
        {EQ, {A_oper::A_eqOp, 5, 6}},
        {NEQ, {A_oper::A_neqOp, 5, 6}},

        {ADD, {A_oper::A_plusOp, 7, 8}},
        {SUB, {A_oper::A_minusOp, 7, 8}},
        {MUL, {A_oper::A_timesOp, 9, 10}},
        {DIV, {A_oper::A_divideOp, 9, 10}}
    };
    if (!result.count(tok.type)) 
        return std::nullopt;
    return result[tok.type];
}

ptr<A_dec> parser::dec() {
    auto t = tok();
    switch (t.type)
    {
    case VAR : 
        {
            auto id = eat(IDENTIFIER);
            S_symbol type("");
            token t = tok();
            if(t.type == COLON) {
                auto id = eat(IDENTIFIER);
                type = id.val;
                eat(ASSIGN);
            }
            else if(t.type != ASSIGN) {
                std::cerr << "in line " << t.line << ":" << std::endl;
                std::cerr << "parser: expected COLON or ASSIGN, but got " << t.to_str() << std::endl;
                exit(1);
            }
            auto e = exp();
            return make_shared<A_VarDec>(t.line, id.val, type, e);
        } 
        break;
    case TYPE :
        {
            token id = eat(IDENTIFIER);
            eat(EQ);
            auto type = ty();
            std::vector<ptr<A_namety>> types = {make_shared<A_namety>(id.val, type)};
            return make_shared<A_TypeDec>(id.line, types);
        }
        break;
    case FUNCTION:
        {
            token func_name = eat(IDENTIFIER);
            eat(L_SMALL);
            auto list = field_list();
            eat(R_SMALL);
            token next = tok();
            if(next.type == EQ) {
                auto e = exp();
                auto func = make_shared<A_funcdec>(t.line, func_name.val, list, "", e);
                std::vector<ptr<A_funcdec>> funcs = {func};
                return make_shared<A_FunctionDec>(func_name.line, funcs);
            }
            else if(next.type == COLON) {
                auto id = eat(IDENTIFIER);
                eat(EQ);
                auto e = exp();
                auto func = make_shared<A_funcdec>(t.line, func_name.val, list, id.val, e);
                std::vector<ptr<A_funcdec>> funcs = {func};
                return make_shared<A_FunctionDec>(func_name.line, funcs);
            }
            else {
                std::cerr << "in line " << next.line << ":" << std::endl;
                std::cerr << "parser: expected EQ or COLON, but got " << next.to_str() << std::endl;
                exit(1);
            }
        }
    default:
        unuse(t);
        break;
    }
    return nullptr;
}

ptr<A_exp> parser::exp() {
    auto t = tok();
    switch (t.type)
    {
    case LET: 
        {
            auto ds = decs();
            eat(IN);
            auto es = seqexp();
            eat(END);
            return make_shared<A_LetExp>(t.line, ds, es);
        }
        break;
    case WHILE:
        {
            auto cond = exp();
            eat(DO);
            auto body = exp();
            return make_shared<A_WhileExp>(t.line, cond, body);
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
            return make_shared<A_ForExp>(t.line, ite.val, start, end, body);
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
                return make_shared<A_IfExp>(t.line, cond, True, nullptr);
            }
            auto False = exp();
            return make_shared<A_IfExp>(t.line, cond, True, False);
        }
        break;
    case BREAK:
        return make_shared<A_BreakExp>(t.line);
        break;
    case L_SMALL:
        {
            auto seq = seqexp();
            eat(R_SMALL);
            return seq;
        }
        break;
    }
    // we can assume that this exp is an exp with operator,
    // and throw it to relevant functions.
    unuse(t);
    return binary_exp(0);
}

ptr<A_exp> parser::binary_exp(int min_binding_power) {
    ptr<A_exp> lhs = nullptr;
    auto token = tok();
    switch (token.type) {
    case SUB:
        lhs = binary_exp(8);
        lhs = make_shared<A_OpExp>(token.line, A_oper::A_minusOp, 
                    make_shared<A_IntExp>(token.line, 0), lhs);
        break;
    default:
        unuse(token);
    };
    if (!lhs) {
        lhs = item();
    }
    while (true) {
        auto expected_operater = tok();
        auto oper = get_operator(expected_operater);
        if (!oper.has_value()) {
            unuse(expected_operater);
            break;
        }
        auto [a_oper, left_binding_power, right_binding_power] = oper.value();
        if (left_binding_power < min_binding_power) {
            unuse(expected_operater);
            break;
        }
        auto rhs = binary_exp(right_binding_power);
        lhs = binding_expr_oper(expected_operater.line, a_oper, lhs, rhs);
    }
    return lhs;
}

ptr<A_exp> parser::item() {
    token t = tok();
    switch (t.type)
    {
    case INT_LITERAL:
        return make_shared<A_IntExp>(t.line, atoi(t.val.c_str()));
    case STR_LITERAL:
        return make_shared<A_StringExp>(t.line, t.val.substr(1, t.val.length()-2));
    case NIL:
        return make_shared<A_NilExp>(t.line);
    case L_SMALL:
        {
            auto seq = seqexp();
            eat(R_SMALL);
            return seq;
        }
    default:
        unuse(t);
    }
    auto ident = tok();
    if (t.type != IDENTIFIER) {
        std::cerr << "in line " << ident.line << std::endl;
        std::cerr << "expected to parse an identifier, but got " << ident.to_str() << std::endl;
        exit(1);
    }
    auto next_token = tok();
    ptr<A_var> result;
    switch (next_token.type) {
    // expected call exp
    case L_SMALL:
        {
            std::vector<ptr<A_exp>> args;
            auto t = tok();
            if (t.type == R_SMALL) {
                return make_shared<A_CallExp>(ident.line, ident.val, args);
            }
            unuse(t);
            while (true) {
                args.push_back(exp());
                auto t = tok();
                if (t.type == SEMICOLON)
                    continue;
                else if (t.type == R_SMALL)
                    break;
                unuse(t);
                eat(R_SMALL); // exit(1)
            }
            return make_shared<A_CallExp>(ident.line, ident.val, args);
        }
    default:
        unuse(next_token);
        result = make_shared<A_SimpleVar>(ident.line, ident.val);
    }
    bool stop = false;
    while (false == stop) {
        auto t = tok();
        switch (t.type) {
        case DOT:
            {
                auto field = eat(IDENTIFIER);
                result = make_shared<A_FieldVar>(ident.line, result, field.val);
            }
            break;
        case L_MID:
            {
                auto offset = exp();
                result = make_shared<A_SubscriptVar>(ident.line, result, offset);
                eat(R_MID);
            }
            break;
        case L_BIG:
            // fall back to record exp
            {
                auto fields = efield_list();
                eat(R_BIG);
                return make_shared<A_RecordExp>(ident.line, ident.val, fields);
            }
        case OF:
            // fall back to array exp
            {
                auto init_value = exp();
                if (result->ty != A_var::type::SUBSCRIPT) {
                    std::cerr << "in line " << t.line << ": unexpected OF" << std::endl;
                }
                auto *subscript_var = dynamic_cast<A_SubscriptVar*>(result.get());
                if (subscript_var->var->ty != A_var::type::SIMPLE) {
                    std::cerr << "in line " << t.line << ": unexpected OF" << std::endl;
                }
                return make_shared<A_ArrayExp>(ident.line, ident.val, subscript_var->exp, init_value);
            }
        default:
            unuse(t);
            stop = true;
        }
    }
    return make_shared<A_VarExp>(ident.line, result);
}

ptr<A_exp> parser::seqexp() {
    token t = tok();
    std::vector<ptr<A_exp>> vec;
    if(t.type == R_SMALL) {
        unuse(t);
        return make_shared<A_SeqExp>(t.line, vec);
    }
    unuse(t);
    A_pos p = 0;
    while(true) {
        auto e = exp();
        p = e->pos;
        vec.push_back(e);
        token t = tok();
        if(t.type == SEMICOLON)
            continue;
        unuse(t);
        break;
    }
    return make_shared<A_SeqExp>(p, vec);
}

std::vector<ptr<A_efield>> parser::efield_list() {
    token t = tok();
    if(t.type == R_BIG) {
        unuse(t);
        return {};
    }
    unuse(t);
    std::vector<ptr<A_efield>> vec;
    while(true) {
        auto ef = efield();
        vec.push_back(ef);
        token t = tok();
        if(t.type == COMMA)
            continue;
        unuse(t);
        break;
    }
    return vec;
}

ptr<A_efield> parser::efield() {
    token id = eat(IDENTIFIER);
    eat(EQ);
    auto e = exp();
    return make_shared<A_efield>(id.val, e);
}

std::vector<ptr<A_field>> parser::field_list() {
    token t = tok();
    if(t.type == R_BIG || t.type == R_SMALL) {
        unuse(t);
        return {};
    }
    unuse(t);
    std::vector<ptr<A_field>> fields;
    while(true) {
        auto f = field();
        fields.push_back(f);
        token t = tok();
        if(t.type == COMMA)
            continue;
        unuse(t);
        break;
    }
    return fields;
}

ptr<A_field> parser::field() {
    token name = eat(IDENTIFIER);
    eat(COLON);
    token type = eat(IDENTIFIER);
    return make_shared<A_field>(name.line, type.val, name.val);
}

ptr<A_ty> parser::ty() {
    token t = tok();
    switch (t.type)
    {
    case IDENTIFIER: return make_shared<A_NameTy>(t.line, t.val);
    case ARRAY:
        {
            eat(OF);
            token id = eat(IDENTIFIER);
            return make_shared<A_ArrayTy>(t.line, id.val);
        }
    case L_BIG:
        {
            auto list = field_list();
            eat(R_BIG);
            return make_shared<A_RecordTy>(t.line, list);
        }
    default:
        std::cerr << "in line " << t.line << ":" << std::endl;
        std::cerr << "parser: expected identifier, array, or {, but got " << t.to_str() << std::endl;
        exit(1);
        break;
    }
    // unreachable
    return nullptr;
}

std::vector<ptr<A_dec>> parser::decs() {
    std::vector<ptr<A_dec>> decs;
    while(true) {
        auto d = dec();
        if(d == nullptr)
            break;
        if(!decs.empty()) {
            if(decs.back()->ty == A_dec::type::FUNCDS && d->ty == A_dec::type::FUNCDS) {
                auto &functions = dynamic_cast<A_FunctionDec*>(decs.back().get())->function;
                functions.push_back(dynamic_cast<A_FunctionDec*>(d.get())->function[0]);
                continue;
            }
            if(decs.back()->ty == A_dec::type::TYDS && d->ty == A_dec::type::TYDS) {
                auto &types = dynamic_cast<A_TypeDec*>(decs.back().get())->type;
                types.push_back(dynamic_cast<A_TypeDec*>(d.get())->type[0]);
                continue;
            }
        }
        decs.push_back(d);
    }
    return decs;
}

ptr<A_exp> parser::parse() {
    return exp();
}
}