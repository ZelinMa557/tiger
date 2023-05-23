#pragma once
#include <string>
#include <vector>
namespace FRONTEND {
const int  WHILE = 1;
const int  FOR = 2;
const int  TO = 3;
const int  BREAK = 4;
const int  LET = 5;
const int  IN = 6;
const int  END = 7;
const int  FUNCTION = 8;
const int  VAR = 9;
const int  TYPE = 10;
const int  ARRAY = 11;
const int  IF = 12;
const int  THEN = 13;
const int  ELSE = 14;
const int  DO = 15;
const int  OF = 16;
const int  NIL = 17;
const int  COMMA = 18;
const int  COLON = 19;
const int  SEMICOLON = 20;
const int  L_SMALL = 21;
const int  R_SMALL = 22;
const int  L_MID = 23;
const int  R_MID = 24;
const int  L_BIG = 25;
const int  R_BIG = 26;
const int  DOT = 27;
const int  ADD = 28;
const int  SUB = 29;
const int  MUL = 30;
const int  DIV = 31;
const int  EQ = 32;
const int  NEQ = 33;
const int  LT = 34;
const int  LE = 35;
const int  GT = 36;
const int  GE = 37;
const int  AND = 38;
const int  OR = 39;
const int  ASSIGN = 40;
const int  INT_TYPE = 41;
const int  STR_TYPE = 42;

const int  IDENTIFIER = 43;
const int  INT_LITERAL = 44;
const int  STR_LITERAL = 45;

class token
{
public:
    const int type;
    const std::string val;
    const int line;
    
    token() = delete;
    token(int t, int l) : type(t), line(l){};
    token(int t, std::string &v, int l) : type(t), val(v), line(l) {};
    token(int t, std::string &&v, int l) : type(t), val(v), line(l) {};
    std::string to_str();
    ~token() = default;
};


// for debug
inline std::string token::to_str() {
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
        case 41: ans = "INT_TYPE"; break;
        case 42: ans = "STR_TYPE"; break;
        case 43: ans = "IDENTIFIER"; break;
        case 44: ans = "INT_LITERAL"; break;
        case 45: ans = "STR_LITERAL"; break;
    }
    if(type > 42) {
        ans += ' ';
        ans += val;
    }
    return ans;
}
}