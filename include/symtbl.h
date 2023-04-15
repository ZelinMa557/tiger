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
    virtual ~tgrTy() {};
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

struct recordTy : public tgrTy {
    std::unordered_map<S_symbol, tgrTy*> fields;
    recordTy(decltype(fields) fs) : tgrTy(TIGTY::RECORD), fields(std::move(fs)) {};
};

struct nameTy : public tgrTy {
    S_symbol pri_type;
    nameTy(S_symbol t) : tgrTy(TIGTY::NAME), pri_type(t) {};
};

struct arrayTy : public tgrTy {
    S_symbol element_type;
    arrayTy(S_symbol t) : tgrTy(TIGTY::ARRAYTY), element_type(t) {};
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
    std::unordered_map<S_symbol, std::vector<tgrTy*>> venv;
    // first element in list is the type of return value
    std::unordered_map<S_symbol, std::vector<std::list<tgrTy*>>> fenv;
    std::vector<stkop> stk;
public:
    symtbl();
    void beginScope();
    void endScope();
    void decType(S_symbol sym, tgrTy* ty);
    void decVar(S_symbol sym, tgrTy* ty);
    void decFunc(S_symbol sym, std::list<tgrTy*> &args, tgrTy* retTy);
    tgrTy* lookTy(S_symbol ty);
    bool ExistTy(S_symbol ty);
    tgrTy* lookVar(S_symbol name);
    std::pair<tgrTy*, std::list<tgrTy*>> lookFunc(S_symbol name);
};