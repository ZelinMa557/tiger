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
            // todo
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
            cout << "IntExp(\"" << e->s <<  "\")";
        }
    default:
        break;
    }
}
