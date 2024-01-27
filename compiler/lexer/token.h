#pragma once
#include <string>
#include <vector>
namespace tiger {
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

const int  IDENTIFIER = 41;
const int  INT_LITERAL = 42;
const int  STR_LITERAL = 43;

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
    ~token() = default;
};
} // namespace tiger