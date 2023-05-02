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
using namespace llvm;
class generator {
private:
    LLVMContext context;
    IRBuilder<> builder;
    std::unique_ptr<Module> module;
    std::unordered_map<std::string, Value *> named_values;
    A_exp *syntax_tree;

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
public:
    generator() = delete;
    generator(A_exp *exp) : syntax_tree(exp) {};

};