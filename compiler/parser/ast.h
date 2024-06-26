#pragma once
#include <string>
#include <vector>
#include <memory>
#include "lexer/token.h"
namespace tiger {
enum class A_oper {A_assignOp, A_andOp, A_orOP, // these three ops are not really appers in ast,
                                                // only used in parsing stage.
                    A_plusOp, A_minusOp, A_timesOp, A_divideOp, 
                    A_eqOp, A_neqOp, A_ltOP, A_leOp, A_gtOp, A_geOp};

template <typename T>
using ptr = std::shared_ptr<T>;

using std::make_shared;

using A_pos = int;
using S_symbol = std::string;
struct A_var;
struct A_exp;
struct A_dec;
struct A_ty;
struct A_field;
struct A_funcdec;
struct A_namety;
struct A_efield;
struct A_var {
public:
    enum class type{SIMPLE, FIELD, SUBSCRIPT};
    A_pos pos;
    type ty;
    A_var(A_pos p, type t) : pos(p), ty(t) {}; 
    virtual ~A_var() {};
};

struct A_SimpleVar : public A_var {
public:
    S_symbol sym;
    A_SimpleVar(A_pos p, S_symbol s) : A_var(p, type::SIMPLE), sym(s) {};
};

struct A_FieldVar : public A_var {
public:
    ptr<A_var> var;
    S_symbol sym;
    A_FieldVar(A_pos p, ptr<A_var> var_, S_symbol s) : A_var(p, type::FIELD), var(var_), sym(s) {};
};

struct A_SubscriptVar : public A_var {
public:
    ptr<A_var> var;
    ptr<A_exp> exp;
    A_SubscriptVar(A_pos p, ptr<A_var> var_, ptr<A_exp> exp_) : 
                            A_var(p, type::SUBSCRIPT), var(var_), exp(exp_) {};
};

struct A_exp {
public:
    enum class type{VarExp, NilExp, IntExp, StringExp, CallExp, OpExp, RecordExp, 
                    SeqExp, AssignExp, IfExp, WhileExp, LetExp, ForExp, ArrayExp, BreakExp};
    A_pos pos;
    type ty;
    A_exp(A_pos p, type t) : pos(p), ty(t) {};
    virtual ~A_exp() {};
};

struct A_VarExp : public A_exp {
public:
    ptr<A_var> var;
    A_VarExp(A_pos p, ptr<A_var> var_) : A_exp(p, type::VarExp), var(var_) {};
};

struct A_NilExp : public A_exp {
public:
    A_NilExp(A_pos p) : A_exp(p, type::NilExp) {};
};

struct A_IntExp : public A_exp {
public:
    int i;
    A_IntExp(A_pos p, int i_) : A_exp(p, type::IntExp), i(i_) {};
};

struct A_StringExp : public A_exp {
public:
    std::string s;
    A_StringExp(A_pos p, std::string s_) : A_exp(p, type::StringExp), s(s_) {};
};

struct A_CallExp : public A_exp {
public:
    S_symbol func;
    std::vector<ptr<A_exp>> args;
    A_CallExp(A_pos p, S_symbol func_, std::vector<ptr<A_exp>> args_) :
                A_exp(p, type::CallExp), func(func_), args(args_) {};
};

struct A_OpExp : public A_exp {
public:
    A_oper oper;
    ptr<A_exp> left;
    ptr<A_exp> right;
    A_OpExp(A_pos p, A_oper oper_, ptr<A_exp> left_, ptr<A_exp> right_) :
        A_exp(p, type::OpExp), oper(oper_), left(left_), right(right_) {};
};

struct A_RecordExp : public A_exp {
public:
    S_symbol type;
    std::vector<ptr<A_efield>> fields;
    A_RecordExp(A_pos p, S_symbol type_, std::vector<ptr<A_efield>> fields_) :
        A_exp(p, type::RecordExp), type(type_), fields(fields_) {};
};

struct A_SeqExp : public A_exp {
public:
    std::vector<ptr<A_exp>> seq;
    A_SeqExp(A_pos p, std::vector<ptr<A_exp>> seq_):
        A_exp(p, type::SeqExp), seq(seq_) {};
};

struct A_AssignExp : public A_exp {
public:
    ptr<A_var> var;
    ptr<A_exp> exp;
    A_AssignExp(A_pos p, ptr<A_var> var_, ptr<A_exp> exp_) :
        A_exp(p, type::AssignExp), var(var_), exp(exp_) {};
};

struct A_IfExp : public A_exp {
public:
    ptr<A_exp> test;
    ptr<A_exp> then;
    ptr<A_exp> elsee;
    A_IfExp(A_pos p, ptr<A_exp> test_, ptr<A_exp> then_, ptr<A_exp> elsee_) :
        A_exp(p, type::IfExp), test(test_), then(then_), elsee(elsee_) {};
};

struct A_WhileExp : public A_exp {
public:
    ptr<A_exp> test;
    ptr<A_exp> body;
    A_WhileExp(A_pos p, ptr<A_exp> test_, ptr<A_exp> body_) :
        A_exp(p, type::WhileExp), test(test_), body(body_) {};
};

struct A_BreakExp : public A_exp {
public:
    A_BreakExp(A_pos p) : A_exp(p, type::BreakExp) {};
};

struct A_ForExp : public A_exp {
public:
    S_symbol var;
    ptr<A_exp> lo;
    ptr<A_exp> hi;
    ptr<A_exp> body;
    A_ForExp(A_pos p, S_symbol var_, ptr<A_exp> lo_, ptr<A_exp> hi_, ptr<A_exp> body_) :
        A_exp(p, type::ForExp), var(var_), lo(lo_), hi(hi_), body(body_) {};
};

struct A_LetExp : public A_exp {
public:
    std::vector<ptr<A_dec>> decs;
    ptr<A_exp> body;
    A_LetExp(A_pos p, std::vector<ptr<A_dec>> decs_, ptr<A_exp> body_) :
        A_exp(p, type::LetExp), decs(decs_), body(body_) {};
};

struct A_ArrayExp : public A_exp {
public:
    S_symbol type;
    ptr<A_exp> size;
    ptr<A_exp> init;
    A_ArrayExp(A_pos p, S_symbol t, ptr<A_exp> size_, ptr<A_exp> init_) :
        A_exp(p, type::ArrayExp), type(t), size(size_), init(init_) {};
};

struct A_dec {
public:
    enum class type{FUNCDS, TYDS, VARD};
    A_pos pos;
    type ty;
    A_dec(A_pos p, type t) : pos(p), ty(t) {};
    virtual ~A_dec() {};
};

struct A_FunctionDec : A_dec {
public:
    std::vector<ptr<A_funcdec>> function;
    A_FunctionDec(A_pos p, std::vector<ptr<A_funcdec>> function_) :
        A_dec(p, type::FUNCDS), function(function_) {}; 
};

struct A_VarDec : A_dec {
public:
    S_symbol var;
    S_symbol type;
    ptr<A_exp> init;
    A_VarDec(A_pos p, S_symbol var_, S_symbol type_, ptr<A_exp> init_) :
        A_dec(p, type::VARD), var(var_), type(type_), init(init_) {};
};

struct A_TypeDec : A_dec {
public:
    std::vector<ptr<A_namety>> type;
    A_TypeDec(A_pos p, std::vector<ptr<A_namety>> type_) :
        A_dec(p, type::TYDS), type(type_) {};
};

struct A_ty {
public:
    enum class type{NameTy, RecordTy, ArrayTy};
    A_pos pos;
    type ty;
    A_ty(A_pos p, type t) : pos(p), ty(t) {};
    virtual ~A_ty() {};
};

struct A_NameTy : public A_ty {
public:
    S_symbol type;
    A_NameTy(A_pos p, S_symbol type_) : A_ty(p, type::NameTy), type(type_) {};
};

struct A_RecordTy : public A_ty {
public:
    std::vector<ptr<A_field>> record;
    A_RecordTy(A_pos p, std::vector<ptr<A_field>> record_) :
        A_ty(p, type::RecordTy), record(record_) {};
};

struct A_ArrayTy : public A_ty {
public:
    S_symbol array;
    A_ArrayTy(A_pos p, S_symbol array_) : A_ty(p, type::ArrayTy), array(array_) {};
};

struct A_field {
public:
    A_pos pos;
    S_symbol type;
    S_symbol name;
    A_field(A_pos pos_, S_symbol type_, S_symbol name_) : pos(pos_), type(type_), name(name_) {};
};

struct A_funcdec {
public:
    A_pos pos;
    S_symbol name;
    std::vector<ptr<A_field>> params;
    S_symbol result;
    ptr<A_exp> body;
    A_funcdec(A_pos p, S_symbol name_, std::vector<ptr<A_field>> params_, S_symbol result_, ptr<A_exp> body_) :
        pos(p), name(name_), params(params_), result(result_), body(body_) {};
};

struct A_namety {
public:
    S_symbol name;
    ptr<A_ty> ty;
    A_namety(S_symbol name_, ptr<A_ty> ty_) : name(name_), ty(ty_) {};
};

struct A_efield {
public:
    S_symbol name;
    ptr<A_exp> exp;
    A_efield(S_symbol name_, ptr<A_exp> exp_) : name(name_), exp(exp_) {};
};
} // namespace tiger