#include "generator.h"
#include <assert.h>
#include <vector>
Value *generator::genExp(A_exp *exp) {
    if(!exp)
        return nullptr;
    switch (exp->ty)
    {
    case A_exp::type::VarExp: return genVarExp(dynamic_cast<A_VarExp*>(exp));
    case A_exp::type::NilExp: return genNilExp(dynamic_cast<A_NilExp*>(exp));
    case A_exp::type::IntExp: return genIntExp(dynamic_cast<A_IntExp*>(exp));
    case A_exp::type::StringExp: return genStringExp(dynamic_cast<A_StringExp*>(exp));
    case A_exp::type::CallExp: return genCallExp(dynamic_cast<A_CallExp*>(exp));
    case A_exp::type::OpExp: return genOpExp(dynamic_cast<A_OpExp*>(exp));
    case A_exp::type::RecordExp: return genRecordExp(dynamic_cast<A_RecordExp*>(exp));
    case A_exp::type::SeqExp: return genSeqExp(dynamic_cast<A_SeqExp*>(exp));
    case A_exp::type::AssignExp: return genAssignExp(dynamic_cast<A_AssignExp*>(exp));
    case A_exp::type::IfExp: return genIfExp(dynamic_cast<A_IfExp*>(exp));
    case A_exp::type::WhileExp: return genWhileExp(dynamic_cast<A_WhileExp*>(exp));
    case A_exp::type::LetExp: return genLetExp(dynamic_cast<A_LetExp*>(exp));
    case A_exp::type::ForExp: return genForExp(dynamic_cast<A_ForExp*>(exp));
    case A_exp::type::ArrayExp: return genArrayExp(dynamic_cast<A_ArrayExp*>(exp));
    case A_exp::type::BreakExp: return genBreakExp(dynamic_cast<A_BreakExp*>(exp));
    }
    assert(0);
    return nullptr;
}

Value *generator::genVarExp(A_VarExp *exp) {
    switch (exp->var->ty)
    {
    case A_var::type::SIMPLE:
        {
            auto var = dynamic_cast<A_SimpleVar*>(exp->var);
            return getNamedValue(var->sym);
        }
    case A_var::type::FIELD:
        {
            auto var = dynamic_cast<A_FieldVar*>(exp->var);
        }
    case A_var::type::SUBSCRIPT:
        {
            auto var = dynamic_cast<A_SubscriptVar*>(exp->var);
        }
    }
}

Value *generator::genNilExp(A_NilExp *exp) {
    return ConstantInt::get(context, APInt(32, 0, true));
}

Value *generator::genIntExp(A_IntExp *exp) {
    return ConstantInt::get(context, APInt(32, exp->i, true));
}

Value *generator::genStringExp(A_StringExp *exp) {
    return getStrConstant(exp->s);
}

Value *generator::genCallExp(A_CallExp *exp) {
    Function *callee = module->getFunction(exp->func);
    std::vector<Value*> args;
    for(auto l = exp->args; l != nullptr && l->head != nullptr; l = l->tail) {
        args.push_back(genExp(l->head));
    }
    return builder.CreateCall(callee, args);
}

Value *generator::genOpExp(A_OpExp *exp) {
    auto lhs = genExp(exp->left);
    auto rhs = genExp(exp->right);
    if(!lhs || !rhs)
        return nullptr;

    switch (exp->oper)
    {
    case A_oper::A_plusOp: return builder.CreateAdd(lhs, rhs);
    case A_oper::A_minusOp: return builder.CreateSub(lhs, rhs);
    case A_oper::A_timesOp: return builder.CreateMul(lhs, rhs);
    case A_oper::A_divideOp: return builder.CreateSDiv(lhs, rhs);
    case A_oper::A_eqOp: return builder.CreateICmpEQ(lhs, rhs);
    case A_oper::A_neqOp: return builder.CreateICmpNE(lhs, rhs);
    case A_oper::A_ltOP: return builder.CreateICmpSLT(lhs, rhs);
    case A_oper::A_leOp: return builder.CreateICmpSLE(lhs, rhs);
    case A_oper::A_gtOp: return builder.CreateICmpSGT(lhs, rhs);
    case A_oper::A_geOp: return builder.CreateICmpSGE(lhs, rhs);
    }
    assert(0);
}

Value *generator::genRecordExp(A_RecordExp *exp) {

}

Value *generator::genSeqExp(A_SeqExp *exp) {
    auto list = exp->seq;
    Value *res = nullptr;
    while(list != nullptr && list->head != nullptr) {
        res = genExp(list->head);
    }
    return res;
}

Value *generator::genAssignExp(A_AssignExp *exp) {

}

Value *generator::genIfExp(A_IfExp *exp) {
    Value *CondV = genExp(exp->test);
    assert(CondV != nullptr);

    Function *TheFunction = builder.GetInsertBlock()->getParent();
    BasicBlock *ThenBB = BasicBlock::Create(context, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(context, "else", TheFunction);
    BasicBlock *MergeBB = BasicBlock::Create(context, "ifcond", TheFunction);
    builder.CreateCondBr(CondV, ThenBB, ElseBB);

    // generate then cond
    builder.SetInsertPoint(ThenBB);
    genExp(exp->then);
    builder.CreateBr(MergeBB);
    builder.SetInsertPoint(MergeBB);

    // generate else cond
    builder.SetInsertPoint(ElseBB);
    genExp(exp->elsee);
    builder.CreateBr(MergeBB);
    builder.SetInsertPoint(MergeBB);

    return nullptr;
}

Value *generator::genWhileExp(A_WhileExp *exp) {
    Function *TheFunction = builder.GetInsertBlock()->getParent();
    BasicBlock *CondBB = BasicBlock::Create(context, "wcond", TheFunction);
    BasicBlock *WhileBodyBB = BasicBlock::Create(context, "wbody", TheFunction);
    BasicBlock *EndBB = BasicBlock::Create(context, "wend", TheFunction);

    builder.CreateBr(CondBB);
    builder.SetInsertPoint(CondBB);
    Value *cond = genExp(exp->test);
    assert(cond != nullptr);
    builder.CreateCondBr(cond, WhileBodyBB, EndBB);

    builder.SetInsertPoint(WhileBodyBB);
    genExp(exp->body);
    builder.CreateBr(CondBB);

    builder.SetInsertPoint(EndBB);

    return nullptr;
}

Value *generator::genLetExp(A_LetExp *exp) {

}

Value *generator::genForExp(A_ForExp *exp) {
    beginScope();
    Function *TheFunction = builder.GetInsertBlock()->getParent();
    BasicBlock *InitBB = BasicBlock::Create(context, "finit", TheFunction);
    BasicBlock *CondBB = BasicBlock::Create(context, "fcond", TheFunction);
    BasicBlock *ForBodyBB = BasicBlock::Create(context, "fbody", TheFunction);
    BasicBlock *EndBB = BasicBlock::Create(context, "fend", TheFunction);

    // init "i"
    builder.CreateBr(InitBB);
    builder.SetInsertPoint(InitBB);
    Value *low = genExp(exp->lo);
    assert(low != nullptr);
    createNamedValue(exp->var, low);
    builder.CreateBr(CondBB);

    builder.SetInsertPoint(CondBB);
    Value *high = genExp(exp->hi);
    assert(high != nullptr);
    Value *CondV = builder.CreateICmpSLE(getNamedValue(exp->var), high);
    builder.CreateCondBr(CondV, ForBodyBB, EndBB);

    builder.SetInsertPoint(ForBodyBB);
    genExp(exp->body);
    // "i++"
    builder.CreateAdd(getNamedValue(exp->var), ConstantInt::get(context, APInt(32, 1, true)));
    builder.CreateBr(CondBB);

    builder.SetInsertPoint(EndBB);
    endScope();
    return nullptr;
}

Value *generator::genArrayExp(A_ArrayExp *exp) {

}

Value *generator::genBreakExp(A_BreakExp *exp) {

}

Value *generator::getStrConstant(std::string &str) {
    Type *charType = Type::getInt8PtrTy(context);
    Constant *strConstant = builder.CreateGlobalStringPtr(str);
    return strConstant;
}

void generator::beginScope() {
    tenv.begin();
    venv.begin();
    fenv.begin();
}

void generator::endScope() {
    tenv.pop();
    venv.pop();
    fenv.pop();
}

void generator::createNamedValue(std::string name, Value *value) {
    venv.put(name, value);
}

Value *generator::getNamedValue(std::string name) {
    return venv.get(name);
}