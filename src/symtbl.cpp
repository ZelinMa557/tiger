#include "symtbl.h"

symtbl::symtbl() {
    auto int_ty = new intTy();
    auto string_ty = new stringTy();
    auto nil_ty = new nilTy();
    auto void_ty = new voidTy();
    tenv["int"].push_back(int_ty);
    tenv["string"].push_back(string_ty);
    tenv["nil"].push_back(nil_ty);
    tenv["void"].push_back(void_ty);

    fenv["print"].push_back({void_ty, string_ty});
    fenv["flush"].push_back({void_ty});
    fenv["getchar"].push_back({string_ty});
    fenv["ord"].push_back({int_ty, string_ty});
    fenv["chr"].push_back({string_ty, int_ty});
    fenv["substring"].push_back({string_ty, string_ty, int_ty, int_ty});
    fenv["concat"].push_back({string_ty, string_ty, string_ty});
    fenv["not"].push_back({int_ty, int_ty});
    fenv["exit"].push_back({void_ty, int_ty});

    fenv["puti"].push_back({void_ty, int_ty});
    fenv["getint"].push_back({int_ty});
    fenv["gets"].push_back({string_ty});
    fenv["size"].push_back({int_ty, string_ty});
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
            if(tenv[name].size()) {
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

void symtbl::decFunc(S_symbol sym, std::list<tgrTy*> &args, tgrTy* retTy) {
    auto args_ = args;
    args_.push_front(retTy);
    fenv[sym].push_back(std::move(args_));
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

std::pair<tgrTy*, std::list<tgrTy*>> symtbl::lookFunc(S_symbol name) {
    if(fenv.count(name)) {
        auto list = fenv[name].back();
        tgrTy *retType = list.front();
        list.pop_front();
        return {retType, list};
    }
    return {nullptr, {}};
}

bool symtbl::ExistTy(S_symbol ty) {
    return tenv.count(ty);
}