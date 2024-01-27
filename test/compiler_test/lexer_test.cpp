#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "lexer/lexer.h"
namespace {
bool compare_tokens(std::vector<tiger::token> &true_result, std::vector<tiger::token> &lexer_result) {
    for (int i = 0; i < true_result.size(); i++) {
        if (true_result[i].type != lexer_result[i].type) {
            return false;
        }
        switch (true_result[i].type) {
        case tiger::STR_LITERAL:
        case tiger::INT_LITERAL:
        case tiger::IDENTIFIER:
            if (true_result[i].val != lexer_result[i].val) {
                return false;
            }
            break;
        }
    }
    return true;
}
} // namespace

std::vector<tiger::token> get_lexer_result(std::string str) {
    std::vector<tiger::token> result;
    tiger::lexer lexer_(std::make_unique<std::istringstream>(str));
    while (true) {
        auto tok = lexer_.next();
        if (tok.type == EOF)
            break;
        result.push_back(tok);
    }
    return result;
}

TEST(lexer, all_kind_token_test) {
    std::string test_str = 
R"(while for to break let in end function var type
array if then else do of nil , : ; ( ) [ ] { } .
+ - * / = <> < <= > >= & | :=
test_identifier 1024 "test string")";
    std::vector<tiger::token> answer;
    for (int i = tiger::WHILE; i <= tiger::ASSIGN; i++) {
        answer.push_back({i, 0});
    }
    answer.push_back({tiger::IDENTIFIER, "test_identifier", 0});
    answer.push_back({tiger::INT_LITERAL, "1024", 0});
    answer.push_back({tiger::STR_LITERAL, "\"test string\"", 0});
    auto lexer_answer = get_lexer_result(test_str);
    ASSERT_EQ(answer.size(), lexer_answer.size());
    ASSERT_TRUE(compare_tokens(answer, lexer_answer));
}

TEST(lexer, comment_test) {
    std::string test_str = R"(
        /* this is a simple comment */
        let while then
        // one line comment
        break function var
        /*
        "multi line comment"
        */
        12345 "str"
    )";
    auto lexer_answer = get_lexer_result(test_str);
    ASSERT_EQ(lexer_answer.size(), 8);
    ASSERT_EQ(lexer_answer[0].type, tiger::LET);
    ASSERT_EQ(lexer_answer[1].type, tiger::WHILE);
    ASSERT_EQ(lexer_answer[2].type, tiger::THEN);
    ASSERT_EQ(lexer_answer[3].type, tiger::BREAK);
    ASSERT_EQ(lexer_answer[4].type, tiger::FUNCTION);
    ASSERT_EQ(lexer_answer[5].type, tiger::VAR);
    ASSERT_EQ(lexer_answer[6].type, tiger::INT_LITERAL);
    ASSERT_EQ(lexer_answer[6].val, "12345");
    ASSERT_EQ(lexer_answer[7].type, tiger::STR_LITERAL);
    ASSERT_EQ(lexer_answer[7].val, "\"str\"");
}

TEST(lexer, expr_test) {
    std::string test_str = "123 >= a & 256 < b | 3 + 4<>10";
    std::vector<tiger::token> answer {
        {tiger::INT_LITERAL, "123", 0},
        {tiger::GE, 0},
        {tiger::IDENTIFIER, "a", 0},
        {tiger::AND, 0},
        {tiger::INT_LITERAL, "256", 0},
        {tiger::LT, 0},
        {tiger::IDENTIFIER, "b", 0},
        {tiger::OR, 0},
        {tiger::INT_LITERAL, "3", 0},
        {tiger::ADD, 0},
        {tiger::INT_LITERAL, "4", 0},
        {tiger::NEQ, 0},
        {tiger::INT_LITERAL, "10", 0}
    };
    auto lexer_result = get_lexer_result(test_str);
    ASSERT_EQ(answer.size(), lexer_result.size());
    ASSERT_TRUE(compare_tokens(answer, lexer_result));
}

TEST(lexer, escape_char_test) {
    std::string test_str = R"("str1\n" "str2\\n" "\tstr3")";
    auto lexer_result = get_lexer_result(test_str);
    ASSERT_EQ(lexer_result.size(), 3);
    ASSERT_EQ(lexer_result[0].val, "\"str1\n\"");
    ASSERT_EQ(lexer_result[1].val, "\"str2\\n\"");
    ASSERT_EQ(lexer_result[2].val, "\"\tstr3\"");
}