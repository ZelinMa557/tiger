#pragma once
#include <string>
#include <vector>
#define WHILE 1
#define FOR 2
#define TO 3
#define BREAK 4
#define LET 5
#define IN 6
#define END 7
#define FUNCTION 8
#define VAR 9
#define TYPE 10
#define ARRAY 11
#define IF 12
#define THEN 13
#define ELSE 14
#define DO 15
#define OF 16
#define NIL 17
#define COMMA 18
#define COLON 19
#define SEMICOLON 20
#define L_SMALL 21
#define R_SMALL 22
#define L_MID 23
#define R_MID 24
#define L_BIG 25
#define R_BIG 26
#define DOT 27
#define ADD 28
#define SUB 29
#define MUL 30
#define DIV 31
#define EQ 32
#define NEQ 33
#define LT 34
#define LE 35
#define GT 36
#define GE 37
#define AND 38
#define OR 39
#define ASSIGN 40

#define IDENTIFIER 41
#define INT_LITERAL 42
#define STR_LITERAL 43

class token
{
public:
    const int type;
    const std::string val;
    
    token() = delete;
    token(int t) : type(t) {};
    token(int t, std::string &v) : type(t), val(v) {};
    token(int t, std::string &&v) : type(t), val(v) {};
    std::string to_str();
    ~token() = default;
};


// for debug
std::string token::to_str() {
    std::string ans;
    switch (type)
    {
        case 1: ans = "WHILE"; break;
        case 2: ans = "FOR"; break;
        case 3: ans = "TO"; break;
        case 4: ans = "BREAK"; break;
        case 5: ans = "LET"; break;
        case 6: ans = "IN"; break;
        case 7: ans = "END"; break;
        case 8: ans = "FUNCTION"; break;
        case 9: ans = "VAR"; break;
        case 10: ans = "TYPE"; break;
        case 11: ans = "ARRAY"; break;
        case 12: ans = "IF"; break;
        case 13: ans = "THEN"; break;
        case 14: ans = "ELSE"; break;
        case 15: ans = "DO"; break;
        case 16: ans = "OF"; break;
        case 17: ans = "NIL"; break;
        case 18: ans = "COMMA"; break;
        case 19: ans = "COLON"; break;
        case 20: ans = "SEMICOLON"; break;
        case 21: ans = "L_SMALL"; break;
        case 22: ans = "R_SMALL"; break;
        case 23: ans = "L_MID"; break;
        case 24: ans = "R_MID"; break;
        case 25: ans = "L_BIG"; break;
        case 26: ans = "R_BIG"; break;
        case 27: ans = "DOT"; break;
        case 28: ans = "ADD"; break;
        case 29: ans = "SUB"; break;
        case 30: ans = "MUL"; break;
        case 31: ans = "DIV"; break;
        case 32: ans = "EQ"; break;
        case 33: ans = "NEQ"; break;
        case 34: ans = "LT"; break;
        case 35: ans = "LE"; break;
        case 36: ans = "GT"; break;
        case 37: ans = "GE"; break;
        case 38: ans = "AND"; break;
        case 39: ans = "OR"; break;
        case 40: ans = "ASSIGN"; break;
        case 41: ans = "IDENTIFIER"; break;
        case 42: ans = "INT_LITERAL"; break;
        case 43: ans = "STR_LITERAL"; break;
    }
    if(type > 40) {
        ans += ' ';
        ans += val;
    }
    return ans;
}