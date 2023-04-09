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

    fenv["print"].push_back({"void", {{"s", "string"}}});
    fenv["flush"].push_back({"void", {}});
    fenv["getchar"].push_back({"string", {}});
    fenv["ord"].push_back({"int", {{"s", "string"}}});
    fenv["chr"].push_back({"string", {{"i", "int"}}});
    fenv["substring"].push_back({"string", {{"s", "string"}, {"first", "int"}, {"n", "int"}}});
    fenv["concat"].push_back({"string", {{"s1", "string"}, {"s2", "string"}}});
    fenv["not"].push_back({"int", {{"i", "int"}}});
    fenv["exit"].push_back({"void", {{"i", "int"}}});
}

void symtbl::beginScope() {
    stk.push_back({operation::BEGIN, ""});
}

void symtbl::endScope() {
    // we assume that beginScope will be called before endScope
    while(stk.back().op != operation::BEGIN) {
        auto name = stk.back().name;
        switch (stk.back().op)
        {
        case operation::VARDEC:
            if(venv[name].size()) {
                venv[name].pop_back();
                if(venv[name].size() == 0)
                    venv.erase(name);
            }
            break;
        case operation::TYDEC:
            if(venv[name].size()) {
                tenv[name].pop_back();
                if(tenv[name].size() == 0)
                    tenv.erase(name);
            }
            break;
        case operation::FUNDEC:
            if(fenv[name].size()) {
                fenv[name].pop_back();
                if(fenv[name].size() == 0)
                    fenv.erase(name);
            }
            break;
        default:
            break;
        }
        stk.pop_back();
    }
    stk.pop_back();
}

void symtbl::decType(S_symbol sym, tgrTy* ty) {
    tenv[sym].push_back(ty);
    stk.push_back({operation::TYDEC, sym});
}

void symtbl::decVar(S_symbol sym, tgrTy* ty) {
    venv[sym].push_back(ty);
    stk.push_back({operation::VARDEC, sym});
}

void symtbl::decFunc(S_symbol sym, std::list<field> &args, S_symbol retTy) {
    fenv[sym].push_back({retTy, std::move(args)});
    stk.push_back({operation::FUNDEC, sym});
}

tgrTy* symtbl::lookTy(S_symbol ty) {
    if(tenv.count(ty))
        return tenv[ty].back();
    return nullptr;
}

tgrTy* symtbl::lookVar(S_symbol name) {
    if(venv.count(name))
        return venv[name].back();
    return nullptr;
}

std::pair<S_symbol, std::list<field>> symtbl::lookFunc(S_symbol name) {
    if(fenv.count(name))
        return fenv[name].back();
    return {};
}