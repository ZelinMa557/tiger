#include "parser.h"
#include "tychecker.h"
#include "generator.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "expected input file name!";
        exit(1);
    }
    std::string input_file{argv[1]}, output_file{"out.bc"};
    if (argc == 3) {
        output_file = std::string{argv[2]};
    }
    FRONTEND::parser p(input_file);
    auto tree = p.parse();
    tychecker c(tree);
    c.check();
    generator g;
    g.generate(tree, output_file);
    return 0;
}