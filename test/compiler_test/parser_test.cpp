#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "parser/ast.h"
#include "parser/parser.h"
using namespace tiger;
ptr<A_exp> get_ast(const std::string &test_str) {
    auto lexer_ = std::make_unique<lexer>(std::make_unique<std::istringstream>(test_str));
    tiger::parser parser_(std::move(lexer_));
    return parser_.parse();
}

TEST(parser, binary_exp_test1) {
    auto ast1 = get_ast("a[0]+6-8*b.Dbg");
    ASSERT_EQ(ast1->ty, A_exp::type::OpExp);
    auto op_exp1 = dynamic_cast<A_OpExp*>(ast1.get());
    ASSERT_EQ(op_exp1->oper, A_oper::A_minusOp);
    ASSERT_EQ(op_exp1->left->ty, A_exp::type::OpExp);
    ASSERT_EQ(op_exp1->right->ty, A_exp::type::OpExp);
    auto op_exp2 = dynamic_cast<A_OpExp*>(op_exp1->left.get());
    ASSERT_EQ(op_exp2->oper, A_oper::A_plusOp);
    auto op_exp3 = dynamic_cast<A_OpExp*>(op_exp1->right.get());
    ASSERT_EQ(op_exp3->oper, A_oper::A_timesOp);
    // test "a[0]"
    ASSERT_EQ(op_exp2->left->ty, A_exp::type::VarExp);
    auto var1 = dynamic_cast<A_VarExp*>(op_exp2->left.get())->var;
    ASSERT_EQ(var1->ty, A_var::type::SUBSCRIPT);
    auto subs_var = dynamic_cast<A_SubscriptVar*>(var1.get());
    ASSERT_EQ(subs_var->exp->ty, A_exp::type::IntExp);
    ASSERT_EQ(dynamic_cast<A_IntExp*>(subs_var->exp.get())->i, 0);
    ASSERT_EQ(subs_var->var->ty, A_var::type::SIMPLE);
    ASSERT_EQ(dynamic_cast<A_SimpleVar*>(subs_var->var.get())->sym, "a");
    // test "6"
    ASSERT_EQ(op_exp2->right->ty, A_exp::type::IntExp);
    ASSERT_EQ(dynamic_cast<A_IntExp*>(op_exp2->right.get())->i, 6);
    // test "8"
    ASSERT_EQ(op_exp3->left->ty, A_exp::type::IntExp);
    ASSERT_EQ(dynamic_cast<A_IntExp*>(op_exp3->left.get())->i, 8);
    // test "b.d"
    ASSERT_EQ(op_exp3->right->ty, A_exp::type::VarExp);
    auto var2 = dynamic_cast<A_VarExp*>(op_exp3->right.get())->var;
    ASSERT_EQ(var2->ty, A_var::type::FIELD);
    auto field_var = dynamic_cast<A_FieldVar*>(var2.get());
    ASSERT_EQ(field_var->sym, "Dbg");
    ASSERT_EQ(field_var->var->ty, A_var::type::SIMPLE);
    ASSERT_EQ(dynamic_cast<A_SimpleVar*>(field_var->var.get())->sym, "b");
}

TEST(parser, seq_test) {
    auto ast1 = get_ast("(1+1;while 4>=3 do print(i))");
    ASSERT_EQ(ast1->ty, A_exp::type::SeqExp);
    auto &seq = dynamic_cast<A_SeqExp*>(ast1.get())->seq;
    ASSERT_EQ(seq.size(), 2);
    auto expected_call = dynamic_cast<A_WhileExp*>(seq[1].get())->body;
    ASSERT_EQ(expected_call->ty, A_exp::type::CallExp);
    auto call_exp = dynamic_cast<A_CallExp*>(expected_call.get());
    ASSERT_EQ(call_exp->func, "print");
}

#define check_int_expr(expr, value) do { \
    ASSERT_EQ(expr->ty, A_exp::type::IntExp);    \
    ASSERT_EQ(dynamic_cast<A_IntExp*>(expr.get())->i, value); \
} while (0);

TEST(parser, misc_test) {
    std::string let_expr = R"(
    let
        type  arrtype = array of int
        var arr1:arrtype := arrtype [10] of 0
    in
        arr1
    end
    )";
    auto ast2 = get_ast(let_expr);
    ASSERT_EQ(ast2->ty, A_exp::type::LetExp);
    auto let_exp = dynamic_cast<A_LetExp*>(ast2.get());
    auto &decs = let_exp->decs;
    ASSERT_EQ(decs.size(), 2);
    ASSERT_EQ(decs[0]->ty, A_dec::type::TYDS);
    auto ty_dec = dynamic_cast<A_TypeDec*>(decs[0].get())->type[0];
    ASSERT_EQ(ty_dec->name, "arrtype");
    ASSERT_EQ(ty_dec->ty->ty, A_ty::type::ArrayTy);
    ASSERT_EQ(decs[1]->ty, A_dec::type::VARD);
    auto var_dec = dynamic_cast<A_VarDec*>(decs[1].get());
    ASSERT_EQ(var_dec->var, "arr1");
    ASSERT_EQ(var_dec->type, "arrtype");
    ASSERT_EQ(var_dec->init->ty, A_exp::type::ArrayExp);
    auto array_exp = dynamic_cast<A_ArrayExp*>(var_dec->init.get());
    ASSERT_EQ(array_exp->type, "arrtype");
    check_int_expr(array_exp->size, 10);
    check_int_expr(array_exp->init, 0);
}

TEST(parser, binary_exp_test2) {
    auto ast1 = get_ast("a := -8 * 7");
    ASSERT_EQ(ast1->ty, A_exp::type::AssignExp);
    auto assign = dynamic_cast<A_AssignExp*>(ast1.get());
    ASSERT_EQ(assign->var->ty, A_var::type::SIMPLE);
    ASSERT_EQ(assign->exp->ty, A_exp::type::OpExp);
    auto op_exp1 = dynamic_cast<A_OpExp*>(assign->exp.get());
    ASSERT_EQ(op_exp1->oper, A_oper::A_minusOp);
    check_int_expr(op_exp1->left, 0);
    ASSERT_EQ(op_exp1->right, A_exp::type::OpExp);
    auto op_exp2 = dynamic_cast<A_OpExp*>(op_exp1->right.get());
    check_int_expr(op_exp2->left, 8);
    check_int_expr(op_exp2->right, 7);
}