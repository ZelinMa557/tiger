#include "tychecker.h"
#include "parser.h"
#include "printabs.h"

#include <iostream>

using namespace std;
using namespace FRONTEND;
int main(int argc, char **argv) {
    if(argc < 2) {
        cerr << "need filename!" <<endl;
    }
    string file_name(argv[1]);
    parser p(file_name);
    auto abs = p.parse();
    tychecker checker(abs);
    checker.check();
    print_exp(abs, 0);
    return 0;
}