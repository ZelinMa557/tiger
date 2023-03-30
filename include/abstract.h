#pragma once
#include <string>
#include <memory>
#include <vector>
#include "token.h"

enum class A_oper {A_plusOp, A_minusOp, A_timesOp, A_divideOp, 
                    A_eqOp, A_neqOp, A_ltOP, A_leOp, A_gtOp, A_geOp};

using A_pos = int;
using S_symbol = std::string;
struct A_var;
struct A_exp;
struct A_dec;
struct A_ty;
struct A_field;
struct A_fieldList;
struct A_expList;
struct A_funcdec;
struct A_funcdecList;
struct A_decList;
struct A_namety;
struct A_nametyList;
struct A_efield;
struct A_efieldList;
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
    std::unique_ptr<A_var> var;
    S_symbol sym;
    A_FieldVar(A_pos p, std::unique_ptr<A_var> var_, S_symbol s) : A_var(p, type::FIELD), var(std::move(var_)), sym(s) {};
};

struct A_SubscriptVar : public A_var {
public:
    std::unique_ptr<A_var> var;
    std::unique_ptr<A_exp> exp;
    A_SubscriptVar(A_pos p, std::unique_ptr<A_var> var_, std::unique_ptr<A_exp> exp_) : 
                                    A_var(p, type::SUBSCRIPT), var(std::move(var_)), exp(std::move(exp_)) {};
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
    std::unique_ptr<A_var> var;
    A_VarExp(A_pos p, std::unique_ptr<A_var> var_) : A_exp(p, type::VarExp), var(std::move(var_)) {};
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
    std::unique_ptr<A_expList> args;
    A_CallExp(A_pos p, S_symbol func_, std::unique_ptr<A_expList> args_) :
                A_exp(p, type::CallExp), func(func_), args(std::move(args_)) {};
};

struct A_OpExp : public A_exp {
public:
    A_oper oper;
    std::unique_ptr<A_exp> left;
    std::unique_ptr<A_exp> right;
    A_OpExp(A_pos p, A_oper oper_, std::unique_ptr<A_exp> left_, std::unique_ptr<A_exp> right_) :
        A_exp(p, type::OpExp), oper(oper_), left(std::move(left_)), right(std::move(right_)) {};
};

struct A_RecordExp : public A_exp {
public:
    S_symbol type;
    std::unique_ptr<A_efieldList> fields;
    A_RecordExp(A_pos p, S_symbol type_, std::unique_ptr<A_efieldList> fields_) :
        A_exp(p, type::RecordExp), type(type_), fields(std::move(fields_)) {};
};

struct A_SeqExp : public A_exp {
public:
    std::unique_ptr<A_expList> seq;
    A_SeqExp(A_pos p, std::unique_ptr<A_expList> seq_):
        A_exp(p, type::SeqExp), seq(std::move(seq_)) {};
};

struct A_AssignExp : public A_exp {
public:
    std::unique_ptr<A_var> var;
    std::unique_ptr<A_exp> exp;
    A_AssignExp(A_pos p, std::unique_ptr<A_var> var_, std::unique_ptr<A_exp> exp_) :
        A_exp(p, type::AssignExp), var(std::move(var_)), exp(std::move(exp_)) {};
};

struct A_IfExp : public A_exp {
public:
    std::unique_ptr<A_exp> test;
    std::unique_ptr<A_exp> then;
    std::unique_ptr<A_exp> elsee;
    A_IfExp(A_pos p, std::unique_ptr<A_exp> test_, std::unique_ptr<A_exp> then_, std::unique_ptr<A_exp> elsee_) :
        A_exp(p, type::IfExp), test(std::move(test_)), then(std::move(then_)), elsee(std::move(elsee_)) {};
};

struct A_WhileExp : public A_exp {
public:
    std::unique_ptr<A_exp> test;
    std::unique_ptr<A_exp> body;
    A_WhileExp(A_pos p, std::unique_ptr<A_exp> test_, std::unique_ptr<A_exp> body_) :
        A_exp(p, type::WhileExp), test(std::move(test_)), body(std::move(body_)) {};
};

struct A_BreakExp : public A_exp {
public:
    A_BreakExp(A_pos p) : A_exp(p, type::BreakExp) {};
};

struct A_ForExp : public A_exp {
public:
    S_symbol var;
    std::unique_ptr<A_exp> lo;
    std::unique_ptr<A_exp> hi;
    std::unique_ptr<A_exp> body;
    A_ForExp(A_pos p, S_symbol var_, std::unique_ptr<A_exp> lo_, std::unique_ptr<A_exp> hi_, std::unique_ptr<A_exp> body_) :
        A_exp(p, type::ForExp), var(var), lo(std::move(lo_)), hi(std::move(hi_)), body(std::move(body_)) {};
};

struct A_LetExp : public A_exp {
public:
    std::unique_ptr<A_decList> decs;
    std::unique_ptr<A_exp> body;
    A_LetExp(A_pos p, std::unique_ptr<A_decList> decs_, std::unique_ptr<A_exp> body_) :
        A_exp(p, type::LetExp), decs(std::move(decs_)), body(std::move(body_)) {};
};

struct A_ArrayExp : public A_exp {
public:
    S_symbol type;
    std::unique_ptr<A_exp> size;
    std::unique_ptr<A_exp> init;
    A_ArrayExp(A_pos p, S_symbol t, std::unique_ptr<A_exp> size_, std::unique_ptr<A_exp> init_) :
        A_exp(p, type::ArrayExp), type(t), size(std::move(size_)), init(std::move(init_)) {};
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
    std::unique_ptr<A_funcdecList> function;
    A_FunctionDec(A_pos p, std::unique_ptr<A_funcdecList> function_) :
        A_dec(p, type::FUNCDS), function(std::move(function_)) {}; 
};

struct A_VarDec : A_dec {
public:
    S_symbol var;
    S_symbol type;
    std::unique_ptr<A_exp> init;
    A_VarDec(A_pos p, S_symbol var_, S_symbol type_, std::unique_ptr<A_exp> init_) :
        A_dec(p, type::VARD), var(var_), type(type_), init(std::move(init_)) {};
};

struct A_TypeDec : A_dec {
public:
    std::unique_ptr<A_nametyList> type;
    A_TypeDec(A_pos p, std::unique_ptr<A_nametyList> type_) :
        A_dec(p, type::TYDS), type(std::move(type_)) {};
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
    std::unique_ptr<A_fieldList> record;
    A_RecordTy(A_pos p, std::unique_ptr<A_fieldList> record_) :
        A_ty(p, type::RecordTy), record(std::move(record_)) {};
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

struct A_fieldList {
public:
    std::unique_ptr<A_field> head;
    std::unique_ptr<A_fieldList> tail;
    A_fieldList(std::unique_ptr<A_field> head_, std::unique_ptr<A_fieldList> tail_) : head(std::move(head_)), tail(std::move(tail_)) {};
};

struct A_expList {
public:
    std::unique_ptr<A_exp> head;
    std::unique_ptr<A_expList> tail;
    A_expList(std::unique_ptr<A_exp> head_, std::unique_ptr<A_expList> tail_) :
        head(std::move(head_)), tail(std::move(tail_)) {};
};

struct A_funcdec {
public:
    A_pos pos;
    S_symbol name;
    std::unique_ptr<A_fieldList> params;
    S_symbol result;
    std::unique_ptr<A_exp> body;
    A_funcdec(A_pos p, S_symbol name_, std::unique_ptr<A_fieldList> params_, S_symbol result_, std::unique_ptr<A_exp> body_) :
        pos(p), name(name_), params(std::move(params_)), result(result_), body(std::move(body_)) {};
};

struct A_funcdecList {
public:
    std::unique_ptr<A_funcdec> head;
    std::unique_ptr<A_funcdecList> tail;
    A_funcdecList(std::unique_ptr<A_funcdec> head_, std::unique_ptr<A_funcdecList> tail_) :
        head(std::move(head_)), tail(std::move(tail_)) {};
};

struct A_decList {
public:
    std::unique_ptr<A_dec> head;
    std::unique_ptr<A_decList> tail;
    A_decList(std::unique_ptr<A_dec> head_, std::unique_ptr<A_decList> tail_) :
        head(std::move(head_)), tail(std::move(tail_)) {};
};

struct A_namety {
public:
    S_symbol name;
    std::unique_ptr<A_ty> ty;
    A_namety(S_symbol name_, std::unique_ptr<A_ty> ty_) : name(name_), ty(std::move(ty_)) {};
};

struct A_nametyList {
public:
    std::unique_ptr<A_namety> head;
    std::unique_ptr<A_nametyList> tail;
    A_nametyList(std::unique_ptr<A_namety> head_, std::unique_ptr<A_nametyList> tail_) :
        head(std::move(head_)), tail(std::move(tail_)) {};
};

struct A_efield {
public:
    S_symbol name;
    std::unique_ptr<A_exp> exp;
    A_efield(S_symbol name_, std::unique_ptr<A_exp> exp_) : name(name_), exp(std::move(exp_)) {};
};

struct A_efieldList {
    std::unique_ptr<A_efield> head;
    std::unique_ptr<A_efieldList> tail;
    A_efieldList(std::unique_ptr<A_efield> head_, std::unique_ptr<A_efieldList> tail_) :
        head(std::move(head_)), tail(std::move(tail_)) {};
};