#include "generator.h"
#include <assert.h>
#include <vector>
#include <iostream>
void error(std::string err) {
    std::cerr << err << std::endl;
    exit(1);
}

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
    auto ptr = genLeftValue(exp->var);
    return convertRightValue(ptr.first);
}

std::pair<Value*, A_ty*> generator::genLeftValue(A_var *var) {
    switch (var->ty)
    {
    case A_var::type::SIMPLE:
        {
            auto v = dynamic_cast<A_SimpleVar*>(var);
            auto vdec = vdecs.get(v->sym);
            return { getNamedValue(v->sym), tdecs.get(vdec->type) };
        }
    case A_var::type::FIELD:
        {
            auto v = dynamic_cast<A_FieldVar*>(var);
            auto parent = genLeftValue(v->var);
            Value *parentValue = convertRightValue(parent.first);
            A_RecordTy *parentTypeDec = dynamic_cast<A_RecordTy*>(parent.second);
            assert(parentTypeDec && parentTypeDec->ty == A_ty::type::RecordTy);
            int idx = getIdxInRecordTy(v->sym, parentTypeDec);
            A_ty *fieldTypeDec = getFieldTypeDec(v->sym, parentTypeDec);
            auto fieldPtr = builder.CreateGEP(parentValue, genIndice({0, idx}));
            return { fieldPtr, fieldTypeDec };
        }
    case A_var::type::SUBSCRIPT:
        {
            auto v = dynamic_cast<A_SubscriptVar*>(var);
            auto parent = genLeftValue(v->var);
            Value *parentValue = convertRightValue(parent.first);
            A_ArrayTy *parentTypeDec = dynamic_cast<A_ArrayTy*>(parent.second);
            assert(parentTypeDec && parentTypeDec->ty == A_ty::type::ArrayTy);
            Value *offset = genExp(v->exp);
            auto elementTyDec = tdecs.get(parentTypeDec->array);
            assert(elementTyDec || (parentTypeDec->array == "int") || (parentTypeDec->array == "string"));
            auto elementPtr = builder.CreateGEP(parentValue, builder.CreateTrunc(offset, builder.getInt32Ty()));
            return { elementPtr, elementTyDec };
        }
    }
    assert(0);
    return {};
}

Value *generator::genNilExp(A_NilExp *exp) {
    return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(NilTy));
}

Value *generator::genIntExp(A_IntExp *exp) {
    return llvm::ConstantInt::get(context, llvm::APInt(64, exp->i, true));
}

Value *generator::genStringExp(A_StringExp *exp) {
    return getStrConstant(exp->s);
}

Value *generator::genCallExp(A_CallExp *exp) {
    Function *callee = fenv.get(exp->func);
    assert(callee != nullptr);
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
    case A_oper::A_eqOp: return builder.CreateZExt(builder.CreateICmpEQ(lhs, rhs), builder.getInt64Ty());
    case A_oper::A_neqOp: return builder.CreateZExt(builder.CreateICmpNE(lhs, rhs), builder.getInt64Ty());
    case A_oper::A_ltOP: return builder.CreateZExt(builder.CreateICmpSLT(lhs, rhs), builder.getInt64Ty());
    case A_oper::A_leOp: return builder.CreateZExt(builder.CreateICmpSLE(lhs, rhs), builder.getInt64Ty());
    case A_oper::A_gtOp: return builder.CreateZExt(builder.CreateICmpSGT(lhs, rhs), builder.getInt64Ty());
    case A_oper::A_geOp: return builder.CreateZExt(builder.CreateICmpSGE(lhs, rhs), builder.getInt64Ty());
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

A_ty *generator::getFieldTypeDec(std::string name, A_RecordTy *ty) {
    auto list = ty->record;
    for(; list != nullptr && list->head != nullptr; list = list->tail) {
        if(list->head->name == name)
            return tdecs.get(list->head->type);
    }
    return nullptr;
}


Value *generator::genRecordExp(A_RecordExp *exp) {
    auto type = tenv.get(exp->type);
    assert(type != nullptr && type->isPointerTy());
    auto elementType = llvm::cast<llvm::PointerType>(type)->getElementType();
    assert(elementType->isStructTy());
    auto structType = llvm::cast<llvm::StructType>(elementType);
    auto size = module->getDataLayout().getTypeAllocSize(structType);
    Value *sz = llvm::ConstantInt::get(Type::getInt64Ty(context), llvm::APInt(64, size));
    std::string allocator{"alloc"};

    Value *ptr = builder.CreateCall(fenv.get(allocator), sz);
    ptr = builder.CreateBitCast(ptr, llvm::cast<llvm::PointerType>(type));
    auto list = exp->fields;
    auto tydec = dynamic_cast<A_RecordTy*>(tdecs.get(exp->type));
    assert(tydec != nullptr);
    for(; list != nullptr && list->head != nullptr; list = list->tail) {
        auto idx = getIdxInRecordTy(list->head->name, tydec);
        auto initVal = genExp(list->head->exp);
        auto fieldTy = getFieldType(list->head->name, tydec);
        assert(fieldTy != nullptr);
        if(initVal->getType() == NilTy) {
            initVal = convertTypedNil(fieldTy);
        }
        auto fieldPtr = builder.CreateGEP(ptr, genIndice({0, idx}));
        builder.CreateStore(initVal, fieldPtr);
    }
    return ptr;
}

Value *generator::genSeqExp(A_SeqExp *exp) {
    auto list = exp->seq;
    Value *res = nullptr;
    while(list != nullptr && list->head != nullptr) {
        res = genExp(list->head);
        list = list->tail;
    }
    return res;
}

Value *generator::genAssignExp(A_AssignExp *exp) {
    assert(exp->var != nullptr);
    auto dst = genLeftValue(exp->var);
    auto val = genExp(exp->exp);
    if(val->getType() == NilTy) {
        // need to deduction type
        if(dst.second == nullptr) { // string type has no dec
            val = convertTypedNil(llvm::PointerType::getUnqual(llvm::Type::getInt8PtrTy(context)));
        }
        // record type
        else if(dst.second->ty == A_ty::type::RecordTy) {
            val = convertTypedNil(
                getFieldType(
                    dynamic_cast<A_FieldVar*>(exp->var)->sym,
                    dynamic_cast<A_RecordTy*>(dst.second)));
        }
        // array type
        else if(dst.second->ty == A_ty::type::ArrayTy) {
            val = convertTypedNil(tenv.get(dynamic_cast<A_ArrayTy*>(dst.second)->array));
        }
    }
    builder.CreateStore(val, dst.first);
    return nullptr;
}

Value *generator::genIfExp(A_IfExp *exp) {
    Value *CondV = genExp(exp->test);
    CondV = builder.CreateICmpNE(CondV, builder.getInt64(0));
    assert(CondV != nullptr);

    Function *TheFunction = builder.GetInsertBlock()->getParent();
    BasicBlock *ThenBB = BasicBlock::Create(context, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(context, "else");
    BasicBlock *MergeBB = BasicBlock::Create(context, "ifcond");
    builder.CreateCondBr(CondV, ThenBB, ElseBB);

    // generate then cond
    builder.SetInsertPoint(ThenBB);
    Value *ThenV = genExp(exp->then);
    builder.CreateBr(MergeBB);
    ThenBB = builder.GetInsertBlock();
    TheFunction->getBasicBlockList().push_back(ElseBB);

    // generate else cond
    builder.SetInsertPoint(ElseBB);
    Value *ElseV = genExp(exp->elsee);
    builder.CreateBr(MergeBB);
    ElseBB = builder.GetInsertBlock();
    
    TheFunction->getBasicBlockList().push_back(MergeBB);
    builder.SetInsertPoint(MergeBB);

    if(exp->elsee == nullptr)
        return nullptr;
    
    if(ThenV == nullptr || ElseV == nullptr)
        return nullptr;

    if(ThenV->getType() == builder.getVoidTy() || ElseV->getType() == builder.getVoidTy()) {
        return nullptr;
    }
    
    llvm::PHINode *node = builder.CreatePHI(ThenV->getType(), 2);
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
    cond = builder.CreateICmpNE(cond, builder.getInt64(0));
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
    return nullptr;
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
    createNamedValue(exp->var, low, Type::getInt64Ty(context));
    vdecs.put(exp->var, new A_VarDec(exp->pos, exp->var, "int", exp->body));
    builder.CreateBr(CondBB);

    builder.SetInsertPoint(CondBB);
    Value *high = genExp(exp->hi);
    assert(high != nullptr);
    Value *CondV = builder.CreateICmpSLE(
        builder.CreateLoad(getNamedValue(exp->var)), high);
    builder.CreateCondBr(CondV, ForBodyBB, EndBB);

    builder.SetInsertPoint(ForBodyBB);
    genExp(exp->body);
    // "i++"
    builder.CreateStore(
        builder.CreateAdd(
                builder.CreateLoad(getNamedValue(exp->var)),
                llvm::ConstantInt::get(context, llvm::APInt(64, 1, true))),
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
    auto elementType = llvm::cast<llvm::PointerType>(type)->getElementType();
    if(initValue->getType() == NilTy) {
        initValue = convertTypedNil(elementType);
    }
    auto size = module->getDataLayout().getTypeAllocSize(elementType);
    Value *sz = llvm::ConstantInt::get(Type::getInt64Ty(context), llvm::APInt(64, size));
    std::string allocator{"alloc"};
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
    Value *index = builder.CreateAlloca(builder.getInt32Ty(), nullptr);
    builder.CreateStore(builder.getInt32(0), index);
    builder.CreateBr(CondBB);

    // "i < len"
    builder.SetInsertPoint(CondBB);
    Value *CondV = builder.CreateICmpSLT(builder.CreateLoad(index), builder.CreateTrunc(arrayLength, builder.getInt32Ty()));
    builder.CreateCondBr(CondV, ForBodyBB, EndBB);

    // "arr[i] = init_val"
    builder.SetInsertPoint(ForBodyBB);
    auto element = builder.CreateGEP(ptr, builder.CreateLoad(index));
    builder.CreateStore(initValue, element);

    // "index++"
    builder.CreateStore(
        builder.CreateAdd(
            builder.CreateLoad(index), 
            llvm::ConstantInt::get(context, llvm::APInt(32, 1, true))),
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
    llvm::Constant *strConstant = builder.CreateGlobalStringPtr(str);
    return strConstant;
}

void generator::genVarDec(A_VarDec *dec) {
    Value *initValue = genExp(dec->init);
    createNamedValue(dec->var, initValue, tenv.get(dec->type));
    vdecs.put(dec->var, dec);
}

void generator::genTypeDec(A_TypeDec *dec) {
    auto l = dec->type;
    // At first, we just focus on record type, because it supports pre declare
    for(; l != nullptr && l->head != nullptr; l = l->tail) {
        auto cur = l->head;
        if(cur->ty->ty == A_ty::type::RecordTy) {
            tenv.put(cur->name, llvm::PointerType::getUnqual(llvm::StructType::create(context, cur->name)));
            tdecs.put(cur->name, cur->ty);
        }
    }

    // Then, we gen array types, which cannot be pre declared
    for(l = dec->type; l != nullptr && l->head != nullptr; l = l->tail) {
        auto cur = l->head;
        if(cur->ty->ty == A_ty::type::ArrayTy) {
            auto t = dynamic_cast<A_ArrayTy*>(cur->ty);
            auto elementType = tenv.get(t->array);
            auto pointerType = llvm::PointerType::getUnqual(elementType);
            tenv.put(cur->name, pointerType);
            tdecs.put(cur->name, cur->ty);
        }
    }

    // In the end, we set body of record type.
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
            auto structPointerType = llvm::cast<llvm::PointerType>(tenv.get(cur->name));
            assert(structPointerType != nullptr && structPointerType->isPointerTy());
            auto structType = llvm::cast<llvm::StructType>(structPointerType->getElementType());
            assert(structType != nullptr && structType->isStructTy());
            structType->setBody(fields);
        }
    }
}

void generator::genFuncDec(A_FunctionDec *dec) {
    auto l = dec->function;
    for(; l != nullptr && l->head != nullptr; l = l->tail) {
        auto cur = l->head;
        Type *retTy = nullptr;
        if(cur->result.length() == 0)
            retTy = llvm::Type::getVoidTy(context);
        else
            retTy = tenv.get(cur->result);
        assert(retTy != nullptr);

        std::vector<Type*> paramTys;
        for(auto params = cur->params; params != nullptr && params->head != nullptr; params = params->tail) {
            Type *t = tenv.get(params->head->type);
            assert(t != nullptr);
            paramTys.push_back(t);
        }
        auto functionType = llvm::FunctionType::get(retTy, paramTys, false);
        auto func = Function::Create(functionType, Function::InternalLinkage, cur->name, module.get());
        fenv.put(cur->name, func);
    }

    for(l = dec->function; l != nullptr && l->head != nullptr; l = l->tail) {
        auto cur = l->head;
        beginScope();
        Function *TheFunction = fenv.get(cur->name);
        assert(TheFunction != nullptr);
        BasicBlock *Body = BasicBlock::Create(context, "entry", TheFunction);
        auto originalPoint = builder.GetInsertPoint();
        auto originalBlock = builder.GetInsertBlock();
        builder.SetInsertPoint(Body);

        auto params = cur->params;
        for(auto &Arg : TheFunction->args()) {
            assert(params && params->head);
            createNamedValue(params->head->name, &Arg, tenv.get(params->head->type));
            vdecs.put(params->head->name, new A_VarDec(params->head->pos, params->head->name, params->head->type, nullptr));
            params = params->tail;
        }
        Value *retVal = genExp(cur->body);
        if(TheFunction->getFunctionType()->getReturnType() != Type::getVoidTy(context))
            builder.CreateRet(retVal);
        else builder.CreateRet(nullptr);
        // if(verifyFunction(*TheFunction, &llvm::outs())) {
        //     error("Generator: Fail to generate function " + cur->name);
        // }
        endScope();
        builder.SetInsertPoint(originalBlock, originalPoint);
    }
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
    auto functionType = llvm::FunctionType::get(ret_ty, arg_tys, false);
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
    vdecs.begin();
}

void generator::endScope() {
    tenv.pop();
    venv.pop();
    fenv.pop();
    tdecs.pop();
    vdecs.pop();
}

void generator::createNamedValue(std::string name, Value *value, Type *type) {
    if(value->getType() == NilTy) {
        value = convertTypedNil(type);
    }
    Value *namedValue = builder.CreateAlloca(value->getType());
    builder.CreateStore(value, namedValue);
    venv.put(name, namedValue);
}

Value *generator::getNamedValue(std::string name) {
    return venv.get(name);
}

Value *generator::convertRightValue(Value *leftValue) {
    return builder.CreateLoad(leftValue);
}

void generator::initFenv() {
    llvm::Type *intType{llvm::Type::getInt64Ty(context)};
    llvm::Type *voidType{llvm::Type::getVoidTy(context)};
    llvm::Type *stringType{llvm::Type::getInt8PtrTy(context)};

    fenv.put("print", createIntrinsicFunction("__print__", {stringType}, voidType));
    fenv.put("puti", createIntrinsicFunction("__puti__", {intType}, voidType));
    fenv.put("flush", createIntrinsicFunction("__flush__", {}, voidType));
    fenv.put("getchar", createIntrinsicFunction("__getchar__", {}, stringType));
    fenv.put("getint", createIntrinsicFunction("__getint__", {}, intType));
    fenv.put("ord", createIntrinsicFunction("__ord__", {stringType}, intType));
    fenv.put("size", createIntrinsicFunction("__size__", {stringType}, intType));
    fenv.put("substring", createIntrinsicFunction("__substring__", {stringType, intType, intType}, stringType));
    fenv.put("concat", createIntrinsicFunction("__concat__", {stringType, stringType}, stringType));
    fenv.put("not", createIntrinsicFunction("__not__", {intType}, intType));
    fenv.put("exit", createIntrinsicFunction("__exit__", {intType}, voidType));
    fenv.put("alloc", createIntrinsicFunction("alloc", {intType}, stringType));
    fenv.put("gets", createIntrinsicFunction("__gets__", {}, stringType));
    fenv.put("chr", createIntrinsicFunction("alloc", {intType}, stringType));
}

void generator::generate(A_exp *syntax_tree, std::string filename, int task) {
    auto mainFunctionType = llvm::FunctionType::get(llvm::Type::getInt64Ty(context), false);
    auto mainFunction =
        llvm::Function::Create(mainFunctionType, llvm::GlobalValue::ExternalLinkage,
                                "main", module.get());
    auto block = BasicBlock::Create(context, "entry", mainFunction);
    builder.SetInsertPoint(block);
    genExp(syntax_tree);
    builder.CreateRet(llvm::ConstantInt::get(Type::getInt64Ty(context), llvm::APInt(64, 0)));
    std::cout<<"success to build, writing...\n";
    std::error_code EC;
    llvm::raw_fd_ostream OS {filename, EC};
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message() << "\n";
        return;
    }
    if (task == 1)
        module->print(OS, nullptr);
    else
        llvm::WriteBitcodeToFile(*module.get(), OS);
    std::cout<<"Done.\n";
}

std::vector<Value*> generator::genIndice(std::vector<int> ids) {
    std::vector<Value*> ret;
    for(auto id : ids)
        ret.push_back(builder.getInt32(id));
    return ret;
}