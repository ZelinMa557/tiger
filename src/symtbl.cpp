#include "symtbl.h"

symtbl::symtbl() {
    auto int_ty = new intTy();
    auto string_ty = new stringTy();
    auto nil_ty = new nilTy();
    auto void_ty = new voidTy();
    tenv["int"].push_back(int_ty);
    tenv["string"].push_back(string_ty);
    tenv["nil"].push_back(nil_ty);
    tenv["void_ty"].push_back(void_ty);

    // todo : add base functions
}

void symtbl::beginScope() {
    stk.push_back({operation::BEGIN, ""});
    t_decs.push({});
    f_decs.push({});
    v_decs.push({});
}

void symtbl::endScope() {
    // we assume that beginScope will be called before endScope
    while(stk.back().op != operation::BEGIN) {
        auto name = stk.back().name;
        switch (stk.back().op)
        {
        case operation::VARDEC:
            if(venv[name].size()) venv[name].pop_back(); break;
        case operation::TYDEC:
            if(venv[name].size()) tenv[name].pop_back(); break;
        case operation::FUNDEC:
            if(fenv[name].size()) fenv[name].pop_back(); break;
        default:
            break;
        }
        stk.pop_back();
    }
    stk.pop_back();
    t_decs.pop();
    f_decs.pop();
    v_decs.pop();
}