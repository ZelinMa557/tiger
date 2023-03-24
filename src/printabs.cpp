#include "abstract.h"
#include "printabs.h"
#include <iostream>
using namespace std;
void space(int n) {
    for(int i = 0; i < n; i++)
        putchar(' ');
}

void print_exp(A_exp* exp, int front_space) {
    space(front_space);
    switch (exp->ty)
    {
    case A_exp::type::NilExp: cout << "NilExp()"; break;
    case A_exp::type::ArrayExp: 
        {
            auto e = dynamic_cast<A_ArrayExp*>(exp);
            cout << "ArrayExp(Symbol(" << e->type << ")," << endl;
            print_exp(e->size.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->init.release(), front_space+4);
            cout << ")" << endl;
        }
        break;
    case A_exp::type::AssignExp:
        {
            auto e = dynamic_cast<A_AssignExp*>(exp);
            cout << "AssignExp(" << endl;
            print_var(e->var.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->exp.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::BreakExp: cout << "BreakExp()" ;
        break;
    case A_exp::type::CallExp:
        {
            auto e = dynamic_cast<A_CallExp*>(exp);
            cout << "CallExp(Symbol(" << e->func << ")," << endl;
            print_expList(e->args.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::ForExp:
        {
            auto e = dynamic_cast<A_ForExp*>(exp);
            cout << "ForExp(Symbol(" << e->var << ")," << endl;
            print_exp(e->lo.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->hi.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->body.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::IfExp:
        {
            auto e = dynamic_cast<A_IfExp*>(exp);
            cout << "IfExp(" << endl;
            print_exp(e->test.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->then.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->elsee.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::IntExp:
        {
            auto e = dynamic_cast<A_IntExp*>(exp);
            cout << "IntExp(" << e->i << ")";
        }
        break;
    case A_exp::type::LetExp:
        {
            auto e = dynamic_cast<A_LetExp*>(exp);
            cout << "LetExp(" << endl;
            print_decList(e->decs.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->body.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::OpExp:
        {
            auto e = dynamic_cast<A_OpExp*>(exp);
            cout << "OpExp(Oper(";
            switch (e->oper)
            {
            case A_oper::A_divideOp: cout << "DIV"; break;
            case A_oper::A_eqOp: cout << "EQ"; break;
            case A_oper::A_geOp: cout << "GE"; break;
            case A_oper::A_gtOp: cout << "GT"; break;
            case A_oper::A_leOp: cout << "LE"; break;
            case A_oper::A_ltOP: cout << "LT"; break;
            case A_oper::A_minusOp: cout << "SUB"; break;
            case A_oper::A_neqOp: cout << "NEQ"; break;
            case A_oper::A_plusOp: cout << "ADD"; break;
            case A_oper::A_timesOp: cout << "MUL"; break;
            }
            cout << ")," << endl;
            print_exp(e->left.release(), front_space+4);
            cout << "," << endl;
            print_exp(e->right.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::RecordExp:
        {
            auto e = dynamic_cast<A_RecordExp*>(exp);
            cout << "RecordExp(Symbol(" << e->type << "),";
            print_efieldList(e->fields.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::SeqExp:
        {
            auto e = dynamic_cast<A_SeqExp*>(exp);
            cout << "SeqExp(" << endl;
            print_expList(e->seq.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_exp::type::StringExp:
        {
            auto e = dynamic_cast<A_StringExp*>(exp);
            cout << "StringExp(\"" << e->s <<  "\")";
        }
    default:
        break;
    }
}

void print_expList(A_expList *expList, int front_space) {
    space(front_space);
    cout << "ExpList(" << endl;
    while(expList != nullptr && expList->head != nullptr) {
        print_exp(expList->head.release(), front_space+4);
        cout << "," << endl;
        expList = expList->tail.release();
    }
    space(front_space);
    cout << ")";
}

void print_var(A_var* var, int front_space) {
    space(front_space);
    switch(var->ty) {
    case A_var::type::SIMPLE:
        {
            auto v = dynamic_cast<A_SimpleVar*>(var);
            cout << "SimpleVar(Symbol(" << v->sym << ")";
        }
        break;
    case A_var::type::FIELD:
        {
            auto v = dynamic_cast<A_FieldVar*>(var);
            cout << "FieldVar(" << endl;
            print_var(v->var.release(), front_space+4);
            cout << "," << endl;
            space(front_space);
            cout << "Symbol(" << v->sym << "))";
        }
        break;
    case A_var::type::SUBSCRIPT:
        {
            auto v = dynamic_cast<A_SubscriptVar*>(var);
            cout << "SubscriptVar(" << endl;
            print_var(v->var.release(), front_space+4);
            cout << "," << endl;
            print_exp(v->exp.release(), front_space+4);
            cout << endl;
        }
    }
}

void print_dec(A_dec* dec, int front_space) {
    space(front_space);
    switch(dec->ty) {
    case A_dec::type::VARD:
        {
            auto d = dynamic_cast<A_VarDec*>(dec);
            cout << "VarDec(Symbol(" << d->var << "),Symbol(" << d->type << ")," << endl;
            print_exp(d->init.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_dec::type::FUNCDS:
        {
            cout << "FunctionDec(" << endl;
            auto d = dynamic_cast<A_FunctionDec*>(dec);
            print_funcdecList(d->function.release(), front_space+4);
            cout << ")";
        }
        break;
    case A_dec::type::TYDS:
        {
            auto d = dynamic_cast<A_TypeDec*>(dec);
            cout << "TypeDec(" << endl;
            print_nametyList(d->type.release(), front_space+4);
            cout << ")";
        }
        break;
    }
}

void print_decList(A_decList* decList, int front_space) {
    space(front_space);
    cout << "DecList(" << endl;
    while(decList != nullptr && decList->head != nullptr) {
        print_dec(decList->head.release(), front_space+4);
        cout << "," << endl;
        decList = decList->tail.release();
    }
    space(front_space);
    cout << ")";
}

void print_ty(A_ty* ty, int front_space) {
    space(front_space);
    switch(ty->ty) {
    case A_ty::type::NameTy:
        {
            auto t = dynamic_cast<A_NameTy*>(ty);
            cout << "NameTy(Symbol(" << t->type << "))";
        }
        break;
    case A_ty::type::ArrayTy:
        {
            auto t = dynamic_cast<A_ArrayTy*>(ty);
            cout << "ArrayTy(Symbol(" << t->array << "))";
        }
        break;
    case A_ty::type::RecordTy:
        {
            auto t = dynamic_cast<A_RecordTy*>(ty);
            cout << "RecordTy(" << endl;
            print_fieldList(t->record.release(), front_space+4);
        }
        break;
    }
}

void print_field(A_field* ty, int front_space) {
    space(front_space);
    cout << "Field(Symbol(" << ty->name << "),Symbol(" << ty->type << "))";
}

void print_fieldList(A_fieldList* fieldList, int front_space) {
    space(front_space);
    cout << "FieldList(" << endl;
    while(fieldList != nullptr && fieldList->head != nullptr) {
        print_field(fieldList->head.release(), front_space+4);
        cout << "," << endl;
        fieldList = fieldList->tail.release();
    }
    space(front_space);
    cout << ")";
}

void print_namety(A_namety* ty, int front_space) {
    space(front_space);
    cout << "namety(Symbol(" << ty->name << ")," << endl;
    print_ty(ty->ty.release(), front_space+4);
    cout << ")";
}

void print_nametyList(A_nametyList* tyList, int front_space) {
    space(front_space);
    cout << "nametyList(" << endl;
    while(tyList != nullptr && tyList->head != nullptr) {
        print_namety(tyList->head.release(), front_space+4);
        cout << "," << endl;
        tyList = tyList->tail.release();
    }
    space(front_space);
    cout << ")";
}

void print_funcdec(A_funcdec* func, int front_space) {
    space(front_space);
    cout << "funcdec(Symbol(" << func->name << ")," << endl;
    print_fieldList(func->params.release(), front_space+4);
    cout << ")";
}

void print_funcdecList(A_funcdecList* funcList, int front_space) {
    space(front_space);
    cout << "funcdecList(" << endl;
    while(funcList != nullptr && funcList->head != nullptr) {
        print_funcdec(funcList->head.release(), front_space+4);
        cout << "," << endl;
        funcList = funcList->tail.release();
    }
    space(front_space);
    cout << ")";
}

void print_efield(A_efield* efield, int front_space) {
    space(front_space);
    cout << "Efield((Symbol(" << efield->name << ")," << endl;
    print_exp(efield->exp.release(), front_space+4);
    cout << ")";
}

void print_efieldList(A_efieldList* efieldList, int front_space) {
    space(front_space);
    cout << "EfieldList(" << endl;
    while(efieldList != nullptr && efieldList->head != nullptr) {
        print_efield(efieldList->head.release(), front_space+4);
        cout << "," << endl;
        efieldList = efieldList->tail.release();
    }
    space(front_space);
    cout << ")";
}