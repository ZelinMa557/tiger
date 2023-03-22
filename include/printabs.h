#pragma once
#include "abstract.h"
void space(int n);
void print_exp(A_exp* exp, int front_space);
void print_expList(A_expList *A_expList, int front_space);
void print_var(A_var* var, int front_space);
void print_dec(A_dec*, int front_space);
void print_decList(A_decList* decList, int front_space);
void print_ty(A_ty* ty, int front_space);
void print_field(A_field* ty, int front_space);
void print_fieldList(A_fieldList* fieldList, int front_space);
void print_namety(A_namety* ty, int front_space);
void print_nametyList(A_nametyList* tyList, int front_space);
void print_funcdec(A_funcdec* func, int front_space);
void print_funcdecList(A_funcdecList* funcList, int front_space);
void print_efield(A_efield* efield, int front_space);
void print_efieldList(A_efieldList* efieldList, int front_space);