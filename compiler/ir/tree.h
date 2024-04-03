#include <memory>
#include <vector>
#include <string>
namespace tiger {
using Temp_label = std::string;
using std::shared_ptr;
using std::make_shared;

enum class T_binOp{T_plus, T_minus, T_mul, T_div,
	      T_and, T_or, T_lshift, T_rshift, T_arshift, T_xor};

enum class T_relOp{T_eq, T_ne, T_lt, T_gt, T_le, T_ge,
		T_ult, T_ule, T_ugt, T_uge};

struct T_exp;
struct T_stm {
    enum class kind {T_SEQ, T_LABEL, T_JUMP, T_CJUMP, T_MOVE, T_EXP};
    kind kind_;
    T_stm(kind kind): kind_(kind) {}
};

struct T_seq : public T_stm {
    shared_ptr<T_stm> left_;
    shared_ptr<T_stm> right_;
    T_seq(shared_ptr<T_stm> left, shared_ptr<T_stm> right):
        T_stm(kind::T_SEQ), left_(left), right_(right) {}
};

struct T_cjump : public T_stm {
    T_relOp op_;
    shared_ptr<T_exp> left_;
    shared_ptr<T_exp> right_;
    Temp_label if_true_;
    Temp_label if_false_;
    T_cjump(T_relOp op, shared_ptr<T_exp> left, 
            shared_ptr<T_exp> right, Temp_label if_true, Temp_label if_false) :
            T_stm(kind::T_CJUMP), op_(op), left_(left), right_(right),
            if_true_(if_true), if_false_(if_false_) {}
};

struct T_move : public T_stm {
    shared_ptr<T_exp> dst_;
    shared_ptr<T_exp> src_;
    T_move(shared_ptr<T_exp> dst, shared_ptr<T_exp> src) :
        T_stm(kind::T_MOVE), dst_(dst), src_(src) {}
};

struct T_label : public T_stm {
    Temp_label label_;
    T_label(Temp_label label) : T_stm(kind::T_LABEL), label_(label) {}
};

struct T_exp : public T_stm {
    enum class exp_kind {T_BINOP, T_MEM, T_TEMP, T_ESEQ, T_NAME,
		      T_CONST, T_CALL};
    exp_kind exp_kind_;
    T_exp(exp_kind exp_kind) : T_stm(kind::T_EXP), exp_kind_(exp_kind) {}
};




// struct T_stm_ {enum {T_SEQ, T_LABEL, T_JUMP, T_CJUMP, T_MOVE,
// 		       T_EXP} kind;
// 	       union {struct {T_stm left, right;} SEQ;
// 		      Temp_label LABEL;
// 		      struct {T_exp exp; Temp_labelList jumps;} JUMP;
// 		      struct {T_relOp op; T_exp left, right;
// 			      Temp_label true, false;} CJUMP;
// 		      struct {T_exp dst, src;} MOVE;
// 		      T_exp EXP;
// 		    } u;
// 	     };

// struct T_exp_ {enum {T_BINOP, T_MEM, T_TEMP, T_ESEQ, T_NAME,
// 		      T_CONST, T_CALL} kind;
// 	      union {struct {T_binOp op; T_exp left, right;} BINOP;
// 		     T_exp MEM;
// 		     Temp_temp TEMP;
// 		     struct {T_stm stm; T_exp exp;} ESEQ;
// 		     Temp_label NAME;
// 		     int CONST;
// 		     struct {T_exp fun; T_expList args;} CALL;
// 		   } u;
// 	    };
} // namespace tiger