#include <unordered_map>
#include <string>
#include <vector>
#include "abstract.h"
enum class TIGTY { RECORD, NILTY, INT, STRING, NAME, ARRAYTY, VOID };
struct tgrTy {
    TIGTY ty;
    tgrTy(TIGTY t) : ty(t) {};
};

struct nilTy : public tgrTy {
    nilTy() : tgrTy(TIGTY::NILTY) {};
};

struct intTy : public tgrTy {
    intTy() : tgrTy(TIGTY::INT) {};
};

struct stringTy : public tgrTy {
    stringTy() : tgrTy(TIGTY::STRING) {};
};

struct voidTy : public tgrTy {
    voidTy() : tgrTy(TIGTY::VOID) {};
};

struct field {
    S_symbol name;
    S_symbol ty;
};

struct recordTy : public tgrTy {
    std::unordered_map<S_symbol, S_symbol> fields;
    recordTy(decltype(fields) fs) : tgrTy(TIGTY::RECORD), fields(std::move(fs)) {};
};

struct nameTy : public tgrTy {
    S_symbol ty;
    nameTy(S_symbol t) : tgrTy(TIGTY::RECORD), ty(t) {};
};

struct arrayTy : public tgrTy {
    S_symbol ty;
    arrayTy(S_symbol t) : tgrTy(TIGTY::RECORD), ty(t) {};
};

class symtbl {
private:
    std::unordered_map<S_symbol, std::vector<tgrTy*>> tenv;
    std::unordered_map<S_symbol, std::vector<S_symbol>> venv;
public:
    symtbl() = delete;
    void beginScope();
    void endScope();
};