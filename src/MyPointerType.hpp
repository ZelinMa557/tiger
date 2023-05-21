#pragma once
#include "generator.h"

class MyPointerType : public Type
{
private:
    Type *PointeeTy;
    unsigned AddressSpace;
public:
    Type *getElementType() const {return PointeeTy; }
    Type *setElementType(Type *ElementType) { PointeeTy = ElementType; }

    bool isPointerTy() { return true; }

    Type *getPointerElementType() const { return getElementType(); }
    unsigned getAddressSpace() const { return AddressSpace; }

    static MyPointerType *get(Type *ElementType, unsigned AddressSpace);
    
    static MyPointerType *get(LLVMContext &C, unsigned AddressSpace);

    static MyPointerType *getUnqual(Type *ElementType) {
        return MyPointerType::get(ElementType, 0);
    }

    static MyPointerType *getUnqual(LLVMContext &C) {
        return MyPointerType::get(C, 0);
    }

    static StructType *create(LLVMContext &Context, StringRef Name) {
        return nullptr;
    }
};