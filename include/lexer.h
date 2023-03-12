#pragma once
#include "token.h"
#include <unordered_map>
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
        {{"&"}, 38}, {{"|"}, 39}, {{":="}, 40}
    };
    enum class state { START, SINGLE_CMT, MULTI_CMT, IDENTI, INT_LITER, STR_LITER };
    state m_state = state::START;
public:
    lexer() = delete;
    lexer(std::string src_file);
    token next();
};