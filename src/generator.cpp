#include "generator.h"
#include "MyPointerType.hpp"
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
    //todo
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
    return ConstantInt::get(context, APInt(64, 0, true));
}

Value *generator::genIntExp(A_IntExp *exp) {
    return ConstantInt::get(context, APInt(64, exp->i, true));
}

Value *generator::genStringExp(A_StringExp *exp) {
    return getStrConstant(exp->s);
}

Value *generator::genCallExp(A_CallExp *exp) {
    // todo
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

int generator::getIdxInRecordTy(std::string name, A_RecordTy *ty) {
    int idx = 0;
    auto list = ty->record;
    for(; list != nullptr && list->head != nullptr; list = list->tail) {
        if(list->head->name == name)
            break;
        idx++;
    }
    return idx;
}

Type *generator::getFieldType(std::string name, A_RecordTy *ty) {
    auto list = ty->record;
    for(; list != nullptr && list->head != nullptr; list = list->tail) {
        if(list->head->name == name)
            return tenv.get(list->head->type);
    }
    return nullptr;
}

Value *generator::genRecordExp(A_RecordExp *exp) {
    auto type = tenv.get(exp->type);
    assert(type != nullptr && type->isPointerTy());
    auto elementType = llvm::cast<llvm::PointerType>(type)->getElementType();
    assert(elementType->isStructTy());
    auto structType = llvm::cast<StructType>(elementType);
    auto size = module->getDataLayout().getTypeAllocSize(structType);
    Value *sz = llvm::ConstantInt::get(Type::getInt64Ty(context), llvm::APInt(64, size));
    std::string allocator{"alloca"};

    Value *ptr = builder.CreateCall(fenv.get(allocator), sz);
    ptr = builder.CreateBitCast(ptr, structType);
    auto list = exp->fields;
    auto tydec = dynamic_cast<A_RecordTy*>(tdecs.get(exp->type));
    assert(tydec != nullptr);
    for(; list != nullptr && list->head != nullptr; list = list->tail) {
        auto idx = getIdxInRecordTy(list->head->name, tydec);
        auto initVal = genExp(list->head->exp);
        auto fieldTy = getFieldType(list->head->name, tydec);
        auto fieldPtr = builder.CreateGEP(structType, ptr, 
                            llvm::ConstantInt::get(llvm::Type::getInt64Ty(context),
                               llvm::APInt(64, idx)));
        builder.CreateStore(initVal, fieldPtr);
    }
    return ptr;
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
    assert(exp->var != nullptr);
    A_VarExp varExp{0, exp->var};
    auto dst = genVarExp(&varExp);
    auto val = genExp(exp->exp);
    builder.CreateStore(val, dst);
    return nullptr;
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
    Value *ThenV = genExp(exp->then);
    builder.CreateBr(MergeBB);
    builder.SetInsertPoint(MergeBB);

    // generate else cond
    builder.SetInsertPoint(ElseBB);
    Value *ElseV = genExp(exp->elsee);
    builder.CreateBr(MergeBB);
    builder.SetInsertPoint(MergeBB);

    if(exp->elsee == nullptr)
        return nullptr;
    
    PHINode *node = builder.CreatePHI(ThenV->getType(), 2, "iftmp");
    node->addIncoming(ThenV, ThenBB);
    node->addIncoming(ElseV, ElseBB);
    return node;
}

Value *generator::genWhileExp(A_WhileExp *exp) {
    Function *TheFunction = builder.GetInsertBlock()->getParent();
    BasicBlock *CondBB = BasicBlock::Create(context, "wcond", TheFunction);
    BasicBlock *WhileBodyBB = BasicBlock::Create(context, "wbody", TheFunction);
    BasicBlock *EndBB = BasicBlock::Create(context, "wend", TheFunction);
    loop_stack.push_back(EndBB);

    builder.CreateBr(CondBB);
    builder.SetInsertPoint(CondBB);
    Value *cond = genExp(exp->test);
    assert(cond != nullptr);
    builder.CreateCondBr(cond, WhileBodyBB, EndBB);

    builder.SetInsertPoint(WhileBodyBB);
    genExp(exp->body);
    builder.CreateBr(CondBB);

    builder.SetInsertPoint(EndBB);
    loop_stack.pop_back();

    return nullptr;
}

Value *generator::genLetExp(A_LetExp *exp) {
    beginScope();
    for(auto l = exp->decs; l != nullptr; l = l->tail) {
        if(l->head != nullptr)
            genDec(l->head);
    }
    genExp(exp->body);
    endScope();
}

Value *generator::genForExp(A_ForExp *exp) {
    beginScope();
    Function *TheFunction = builder.GetInsertBlock()->getParent();
    BasicBlock *InitBB = BasicBlock::Create(context, "finit", TheFunction);
    BasicBlock *CondBB = BasicBlock::Create(context, "fcond", TheFunction);
    BasicBlock *ForBodyBB = BasicBlock::Create(context, "fbody", TheFunction);
    BasicBlock *EndBB = BasicBlock::Create(context, "fend", TheFunction);
    loop_stack.push_back(EndBB);

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
    builder.CreateStore(
        builder.CreateAdd(getNamedValue(exp->var), ConstantInt::get(context, APInt(64, 1, true))),
        getNamedValue(exp->var));
    builder.CreateBr(CondBB);

    builder.SetInsertPoint(EndBB);
    endScope();
    loop_stack.pop_back();
    return nullptr;
}

Value *generator::genArrayExp(A_ArrayExp *exp) {
    auto type = tenv.get(exp->type);
    assert(type != nullptr && type->isPointerTy());
    auto arrayLength = genExp(exp->size);
    auto initValue = genExp(exp->init);

    // allocate space for the array
    auto elementType = llvm::cast<MyPointerType>(type)->getElementType();
    auto size = module->getDataLayout().getTypeAllocSize(elementType);
    Value *sz = llvm::ConstantInt::get(Type::getInt64Ty(context), llvm::APInt(64, size));
    std::string allocator{"alloca"};
    Value *ptr = builder.CreateCall(fenv.get(allocator), builder.CreateMul(arrayLength, sz));
    ptr = builder.CreateBitCast(ptr, type, "array");

    // init the array in a for loop style
    Function *TheFunction = builder.GetInsertBlock()->getParent();
    BasicBlock *InitBB = BasicBlock::Create(context, "finit", TheFunction);
    BasicBlock *CondBB = BasicBlock::Create(context, "fcond", TheFunction);
    BasicBlock *ForBodyBB = BasicBlock::Create(context, "fbody", TheFunction);
    BasicBlock *EndBB = BasicBlock::Create(context, "fend", TheFunction);

    // init "i"
    builder.CreateBr(InitBB);
    builder.SetInsertPoint(InitBB);
    Value *index = builder.CreateAlloca(builder.getInt64Ty(), nullptr);
    builder.CreateStore(builder.getInt64(0), index);
    builder.CreateBr(CondBB);

    // "i < len"
    builder.SetInsertPoint(CondBB);
    Value *CondV = builder.CreateICmpSLT(index, arrayLength);
    builder.CreateCondBr(CondV, ForBodyBB, EndBB);

    // "arr[i] = init_val"
    builder.SetInsertPoint(ForBodyBB);
    auto element = builder.CreateGEP(elementType, ptr, index);
    builder.CreateStore(initValue, element);

    // "index++"
    builder.CreateStore(
        builder.CreateAdd(index, ConstantInt::get(context, APInt(64, 1, true))),
        index);
    builder.CreateBr(CondBB);
    builder.SetInsertPoint(EndBB);
    return ptr;
}

Value *generator::genBreakExp(A_BreakExp *exp) {
    // ignore invalid break
    if(!loop_stack.empty()) {
        builder.CreateBr(loop_stack.back());
        Function *TheFunction = builder.GetInsertBlock()->getParent();
        BasicBlock *BreakCondBB = BasicBlock::Create(context, "break", TheFunction);
        builder.SetInsertPoint(BreakCondBB);
    }
    return nullptr;
}

Value *generator::getStrConstant(std::string &str) {
    Type *charType = Type::getInt8PtrTy(context);
    Constant *strConstant = builder.CreateGlobalStringPtr(str);
    return strConstant;
}

void generator::genVarDec(A_VarDec *dec) {
    Value *initValue = genExp(dec->init);
    createNamedValue(dec->var, initValue);
}

void generator::genTypeDec(A_TypeDec *dec) {
    auto l = dec->type;
    for(; l != nullptr && l->head != nullptr; l = l->tail) {
        auto cur = l->head;
        if(cur->ty->ty == A_ty::type::RecordTy) {
            tenv.put(cur->name, llvm::StructType::create(context, cur->name));
            tdecs.put(cur->name, cur->ty);
        } else if(cur->ty->ty == A_ty::type::ArrayTy) {
            tenv.put(cur->name, MyPointerType::create(context));
            tdecs.put(cur->name, cur->ty);
        }
    }

    for(l = dec->type; l != nullptr && l->head != nullptr; l = l->tail) {
        auto cur = l->head;
        if(cur->ty->ty == A_ty::type::RecordTy) {
            auto t = dynamic_cast<A_RecordTy*>(cur->ty);
            std::vector<Type*> fields;
            for(auto l = t->record; l != nullptr; l = l->tail) {
                if(l->head == nullptr)
                    break;
                if(l->head->type == "int")
                    fields.push_back(llvm::Type::getInt64Ty(context));
                else if(l->head->type == "string")
                    fields.push_back(llvm::Type::getInt8PtrTy(context));
                else
                    fields.push_back(tenv.get(l->head->type));
            }
            auto structType = llvm::cast<StructType>(tenv.get(cur->name));
            assert(structType != nullptr && structType->isStructTy());
            structType->setBody(fields);
        } else if(cur->ty->ty == A_ty::type::ArrayTy) {
            auto t = dynamic_cast<A_ArrayTy*>(cur->ty);
            auto elementType = tenv.get(cur->name);
            auto pointerType = llvm::cast<MyPointerType>(tenv.get(cur->name));
            assert(elementType != nullptr && pointerType != nullptr && pointerType->isPointerTy());
            pointerType->setElementType(elementType);
        }
    }
}

void generator::genFuncDec(A_FunctionDec *dec) {
//todo
}

void generator::genDec(A_dec *dec) {
    switch (dec->ty)
    {
    case A_dec::type::VARD: genVarDec(dynamic_cast<A_VarDec*>(dec)); break;
    case A_dec::type::FUNCDS: genFuncDec(dynamic_cast<A_FunctionDec*>(dec)); break;
    case A_dec::type::TYDS: genTypeDec(dynamic_cast<A_TypeDec*>(dec)); break;
    }
}

Function *generator::createIntrinsicFunction(std::string name, std::vector<Type*> const &arg_tys, Type *ret_ty) {
    auto functionType = FunctionType::get(ret_ty, arg_tys, false);
    auto func = Function::Create(functionType, Function::ExternalLinkage, name, module.get());
    return func;
}

Value *generator::convertTypedNil(Type *type) {
    return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(type));
}

void generator::beginScope() {
    tenv.begin();
    venv.begin();
    fenv.begin();
    tdecs.begin();
}

void generator::endScope() {
    tenv.pop();
    venv.pop();
    fenv.pop();
    tdecs.pop();
}

void generator::createNamedValue(std::string name, Value *value) {
    Value *namedValue = builder.CreateAlloca(value->getType(), value);
    venv.put(name, namedValue);
}

Value *generator::getNamedValue(std::string name) {
    return venv.get(name);
}

void generator::initFenv() {
    llvm::Type *intType{llvm::Type::getInt64Ty(context)};
    llvm::Type *voidType{llvm::Type::getVoidTy(context)};
    llvm::Type *stringType{llvm::Type::getInt8PtrTy(context)};

    fenv.put("print", createIntrinsicFunction("__print__", {stringType}, voidType));
    fenv.put("puti", createIntrinsicFunction("__puti__", {intType}, voidType));
    fenv.put("flush", createIntrinsicFunction("__flush__", {}, voidType));
    fenv.put("getchar", createIntrinsicFunction("__getchar__", {}, voidType));
    fenv.put("getint", createIntrinsicFunction("__getint__", {}, intType));
    fenv.put("ord", createIntrinsicFunction("__ord__", {stringType}, intType));
    fenv.put("size", createIntrinsicFunction("__size__", {stringType}, intType));
    fenv.put("substring", createIntrinsicFunction("__substring__", {stringType, intType, intType}, stringType));
    fenv.put("concat", createIntrinsicFunction("__concat__", {stringType, stringType}, stringType));
    fenv.put("not", createIntrinsicFunction("__not__", {intType}, voidType));
    fenv.put("exit", createIntrinsicFunction("__exit__", {intType}, voidType));
    fenv.put("alloc", createIntrinsicFunction("alloc", {intType}, stringType));
}


// todo: tydec.