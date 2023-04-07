#pragma once
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "abstract.h"
enum class TIGTY { RECORD, NILTY, INT, STRING, NAME, ARRAYTY, VOID };
struct tgrTy {
    TIGTY ty;
    tgrTy(TIGTY t) : ty(t) {};
};

struct nilTy : public tgrTy {
    nilTy() : tgrTy(TIGTY::NILTY) {};
};

struct intTy : public tgrTy {
    intTy() : tgrTy(TIGTY::INT) {};
};

struct stringTy : public tgrTy {
    stringTy() : tgrTy(TIGTY::STRING) {};
};

struct voidTy : public tgrTy {
    voidTy() : tgrTy(TIGTY::VOID) {};
};

struct field {
    S_symbol name;
    S_symbol ty;
};

struct recordTy : public tgrTy {
    std::unordered_map<S_symbol, S_symbol> fields;
    recordTy(decltype(fields) fs) : tgrTy(TIGTY::RECORD), fields(std::move(fs)) {};
};

struct nameTy : public tgrTy {
    S_symbol ty;
    nameTy(S_symbol t) : tgrTy(TIGTY::RECORD), ty(t) {};
};

struct arrayTy : public tgrTy {
    S_symbol ty;
    arrayTy(S_symbol t) : tgrTy(TIGTY::RECORD), ty(t) {};
};

class symtbl {
    enum class operation { BEGIN, VARDEC, TYDEC, FUNDEC };
    struct stkop {
        operation op;
        S_symbol name;
    };
private:
    // here we use vector as stack
    std::unordered_map<S_symbol, std::vector<tgrTy*>> tenv;
    std::unordered_map<S_symbol, std::vector<S_symbol>> venv;
    std::unordered_map<S_symbol, std::vector<std::pair<S_symbol, std::list<field>>>> fenv;
    std::vector<stkop> stk;
public:
    symtbl();
    void beginScope();
    void endScope();
    void decType(S_symbol sym, tgrTy* ty);
    void decVar(S_symbol sym, S_symbol ty);
    void decFunc(S_symbol sym, std::list<field> &args, S_symbol retTy);
    tgrTy* lookTy(S_symbol ty);
};