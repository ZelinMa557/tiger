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
#include "llvm/Bitcode/BitcodeWriter.h"

#include "abstract.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>

template <typename T>
class tbl {
private:
    std::unordered_map<std::string, std::vector<T*>> store;
    std::vector<std::string> q;
public:
    void put(std::string key, T *value) { assert(key != ""); store[key].push_back(value); q.push_back(key); };
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
using Value = llvm::Value;
using Type = llvm::Type;
using Function = llvm::Function;
using BasicBlock = llvm::BasicBlock;
class generator {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::vector<BasicBlock*> loop_stack;

    tbl<Value> venv;
    tbl<Type> tenv;
    tbl<A_ty> tdecs;
    tbl<A_VarDec> vdecs;
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
    void createNamedValue(std::string name, Value *value, Type *type);
    Value *getNamedValue(std::string name);
    int getIdxInRecordTy(std::string name, A_RecordTy *ty);
    Type *getFieldType(std::string name, A_RecordTy *ty);
    A_ty *getFieldTypeDec(std::string name, A_RecordTy *ty);
    Value *convertTypedNil(Type *type);
    Value *convertRightValue(Value *leftValue);

    Function *createIntrinsicFunction(std::string name, std::vector<Type*> const &arg_tys, Type *ret_ty);
    std::pair<Value*, A_ty*> genLeftValue(A_var *vare);

    Type *NilTy = llvm::PointerType::getUnqual(Type::getVoidTy(context));

    std::vector<Value*> genIndice(std::vector<int> ids);

public:
    generator() : builder(context), module(new llvm::Module("The Module", context)) {
        initFenv();
        tenv.put("int", Type::getInt64Ty(context));
        tenv.put("string", llvm::Type::getInt8PtrTy(context));
        tenv.put("void", Type::getVoidTy(context));
    };
    void generate(A_exp *syntax_tree, std::string filename, int task);
};