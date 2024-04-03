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