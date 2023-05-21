#pragma once
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "abstract.h"
#include <memory>
#include <unordered_map>
#include <vector>
using namespace llvm;
template <typename T>
class tbl {
private:
    std::unordered_map<std::string, std::vector<T*>> store;
    std::vector<std::string> q;
public:
    void put(std::string key, T *value) { store[key].push_back(value); };
    T *get(std::string &key) {
        if(store.count(key))
            return store[key].back();
        return nullptr;
    };
    void pop() {
        while(q.size() && q.back() != "") {
            store[q.back()].pop_back();
            if(store[q.back()].empty())
                store.erase(q.back());
            q.pop_back();
        }
        if(!q.empty())
            q.pop_back();
    };
    void begin() { q.push_back(""); };
};

class generator {
private:
    LLVMContext context;
    IRBuilder<> builder;
    std::unique_ptr<Module> module;
    A_exp *syntax_tree;
    std::vector<BasicBlock*> loop_stack;

    tbl<Value> venv;
    tbl<Type> tenv;
    tbl<A_ty> tdecs;
    tbl<Function> fenv;

    Value *genExp(A_exp *exp);
    Value *genVarExp(A_VarExp *exp);
    Value *genNilExp(A_NilExp *exp);
    Value *genIntExp(A_IntExp *exp);
    Value *genStringExp(A_StringExp *exp);
    Value *genCallExp(A_CallExp *exp);
    Value *genOpExp(A_OpExp *exp);
    Value *genRecordExp(A_RecordExp *exp);
    Value *genSeqExp(A_SeqExp *exp);
    Value *genAssignExp(A_AssignExp *exp);
    Value *genIfExp(A_IfExp *exp);
    Value *genWhileExp(A_WhileExp *exp);
    Value *genLetExp(A_LetExp *exp);
    Value *genForExp(A_ForExp *exp);
    Value *genArrayExp(A_ArrayExp *exp);
    Value *genBreakExp(A_BreakExp *exp);

    void genVarDec(A_VarDec *dec);
    void genTypeDec(A_TypeDec *dec);
    void genFuncDec(A_FunctionDec *dec);
    void genDec(A_dec *dec);

    // aux funcs
    void beginScope();
    void endScope();
    void initFenv();
    Value *getStrConstant(std::string &str);
    void createNamedValue(std::string name, Value *value);
    Value *getNamedValue(std::string name);
    int getIdxInRecordTy(std::string name, A_RecordTy *ty);
    Type *getFieldType(std::string name, A_RecordTy *ty);
    Value *convertTypedNil(Type *type);

    Function *createIntrinsicFunction(std::string name, std::vector<Type*> const &arg_tys, Type *ret_ty);
    Value *genLeftValue(A_var *var);

public:
    generator() = delete;
    generator(A_exp *exp) : builder(context), syntax_tree(exp) {};

};