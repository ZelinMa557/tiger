#include "parser.h"
#include "printabs.h"

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    if(argc < 2) {
        cerr << "need filename!" <<endl;
    }
    string file_name(argv[1]);
    parser p(file_name);
    auto abs = p.parse();
    print_exp(abs.release(), 0);
    return 0;
}