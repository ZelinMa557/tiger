#pragma once
#include "generator.h"

class MyPointerType : public Type
{
private:
    Type *PointeeTy;
    explicit MyPointerType(Type *E, unsigned AddrSpace) : Type(E->getContext(), PointerTyID), PointeeTy(E) {
        ContainedTys = &PointeeTy;
        NumContainedTys = 1;
        setSubclassData(AddrSpace);
    };

    explicit MyPointerType(LLVMContext &C, unsigned AddrSpace) : Type(C, PointerTyID), PointeeTy(nullptr) {
        setSubclassData(AddrSpace);
    }
public:
    Type *getElementType() const {return PointeeTy; }
    Type *setElementType(Type *ElementType) { 
        PointeeTy = ElementType;
        ContainedTys = &PointeeTy;
        NumContainedTys = 1;
    }

    Type *getPointerElementType() const { return getElementType(); }
    unsigned getAddressSpace() const { return getSubclassData(); }

    static MyPointerType *get(Type *ElementType, unsigned AddressSpace);
    
    static MyPointerType *get(LLVMContext &C, unsigned AddressSpace);

    static MyPointerType *getUnqual(Type *ElementType) {
        return MyPointerType::get(ElementType, 0);
    }

    static MyPointerType *getUnqual(LLVMContext &C) {
        return MyPointerType::get(C, 0);
    }
    
    static MyPointerType *create(LLVMContext &Context) {
        return get(Context, 0);
    }
};