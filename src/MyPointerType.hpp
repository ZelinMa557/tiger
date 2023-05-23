#pragma once
#include "generator.h"

class MyPointerType : public llvm::Type
{
private:
    Type *PointeeTy;
    explicit MyPointerType(Type *E, unsigned AddrSpace) : Type(E->getContext(), PointerTyID), PointeeTy(E) {
        ContainedTys = &PointeeTy;
        NumContainedTys = 1;
        setSubclassData(AddrSpace);
    };

    explicit MyPointerType(llvm::LLVMContext &C, unsigned AddrSpace) : Type(C, PointerTyID), PointeeTy(nullptr) {
        setSubclassData(AddrSpace);
    }
public:
    Type *getElementType() const {return PointeeTy; }
    void setElementType(Type *ElementType) { 
        PointeeTy = ElementType;
        ContainedTys = &PointeeTy;
        NumContainedTys = 1;
    }

    Type *getPointerElementType() const { return getElementType(); }
    unsigned getAddressSpace() const { return getSubclassData(); }

    static MyPointerType *get(Type *ElementType, unsigned AddressSpace) {
        return new MyPointerType(ElementType, AddressSpace);
    };
    
    static MyPointerType *get(llvm::LLVMContext &C, unsigned AddressSpace) {
        return new MyPointerType(C, AddressSpace);
    };

    static MyPointerType *getUnqual(Type *ElementType) {
        return MyPointerType::get(ElementType, 0);
    }

    static MyPointerType *getUnqual(llvm::LLVMContext &C) {
        return MyPointerType::get(C, 0);
    }
    
    static MyPointerType *create(llvm::LLVMContext &Context) {
        return get(Context, 0);
    }

    static bool classof(const Type *T) {
        return T->getTypeID() == PointerTyID;
    }
};