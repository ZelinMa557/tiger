#pragma once
#include "lexer/token.h"
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <memory>
namespace tiger {
class lexer
{
private:
    std::unordered_map<std::string, int> convert {
        {{"while"}, WHILE}, {{"for"}, FOR}, {{"to"}, TO}, {{"break"}, BREAK}, {{"let"}, LET},
        {{"in"}, IN}, {{"end"}, END}, {{"function"}, FUNCTION}, {{"var"}, VAR}, {{"type"}, TYPE},
        {{"array"}, ARRAY}, {{"if"}, IF}, {{"then"}, THEN}, {{"else"}, ELSE}, {{"do"}, DO},
        {{"of"}, OF}, {{"nil"}, NIL},
        {{","}, COMMA}, {{":"}, COLON}, {{";"}, SEMICOLON}, {{"("}, L_SMALL}, {{")"}, R_SMALL},
        {{"["}, L_MID}, {{"]"}, R_MID}, {{"{"}, L_BIG}, {{"}"}, R_BIG}, {{"."}, DOT},
        {{"+"}, ADD}, {{"-"}, SUB}, {{"*"}, MUL}, {{"/"}, DIV}, {{"="}, EQ},
        {{"<>"}, NEQ}, {{"<"}, LT}, {{"<="}, LE}, {{">"}, GT}, {{">="}, GE},
        {{"&"}, AND}, {{"|"}, OR}, {{":="}, ASSIGN},
    };
    std::unordered_set<char> special{',', ':', ';', '+', '-', '*', '/', '(', ')', '[', ']',
                                        '{', '}', '=', '<', '>', '&', '|', '.'};
    std::unique_ptr<std::basic_istream<char>> input_stream;
    int line_cnt = 1;
    inline char get_next_char() { return input_stream->get(); };
    inline void back_char() { input_stream->seekg(-1, std::ios::cur); };
    std::string next_word();
    void escape_comment();
    std::string get_num(char ch);
    std::string get_identifier(char ch);
    std::string get_operator(char ch);
    std::string get_string();
public:
    lexer() = delete;
    ~lexer() = default;
    lexer(std::unique_ptr<std::basic_istream<char>> input) : input_stream(std::move(input)) {}
    token next();
};
} // namespace tiger