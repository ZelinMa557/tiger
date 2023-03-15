#pragma once
#include "token.h"
#include <unordered_map>
#include <unordered_set>
#include <fstream>
class lexer
{
private:
    std::unordered_map<std::string, int> convert {
        {{"while"}, 1}, {{"for"}, 2}, {{"to"}, 3}, {{"break"}, 4}, {{"let"}, 5},
        {{"in"}, 6}, {{"end"}, 7}, {{"function"}, 8}, {{"var"}, 9}, {{"type"}, 10},
        {{"array"}, 11}, {{"if"}, 12}, {{"then"}, 13}, {{"else"}, 14}, {{"do"}, 15},
        {{"of"}, 16}, {{"nil"}, 17},
        {{","}, 18}, {{":"}, 19}, {{";"}, 20}, {{"("}, 21}, {{")"}, 22},
        {{"["}, 23}, {{"]"}, 24}, {{"{"}, 25}, {{"}"}, 26}, {{"."}, 27},
        {{"+"}, 28}, {{"-"}, 29}, {{"*"}, 30}, {{"/"}, 31}, {{"="}, 32},
        {{"<>"}, 33}, {{"<"}, 34}, {{"<="}, 35}, {{">"}, 36}, {{">="}, 37},
        {{"&"}, 38}, {{"|"}, 39}, {{":="}, 40},
        {{"int"}, 41}, {{"string"}, 42}
    };
    std::unordered_set<char> special{',', ':', ';', '+', '-', '*', '/', '(', ')', '[', ']',
                                        '{', '}', '=', '<', '>', '&', '|', '.'};
    enum class state { START, SINGLE_CMT, MULTI_CMT, IDENTI, INT_LITER, STR_LITER, OPERATOR };
    state m_state = state::START;
    std::fstream src;
    int line_cnt = 1;
    inline char get_next_char() { return src.get(); };
    inline void back_char() { src.seekg(-1, std::ios::cur); };
    std::string next_word();
    void escape_comment();
    std::string get_num(char ch);
    std::string get_identifier(char ch);
    std::string get_operator(char ch);
    std::string get_string();
public:
    lexer() = delete;
    ~lexer();
    lexer(std::string src_file);
    token next();
};