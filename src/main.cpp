#include "parser.h"
#include "tychecker.h"
#include "generator.h"
#include "printabs.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "expected input file name!";
        exit(1);
    }
    std::string input_file{argv[1]}, output_file{"out.bc"};
    if (argc >= 3) {
        output_file = std::string{argv[2]};
    }
    FRONTEND::parser p(input_file);
    auto tree = p.parse();
    if (argc == 3 || strcmp(argv[3], "-bc") == 0) {
        tychecker c(tree);
        c.check();
        generator g;
        g.generate(tree, output_file, 0);
    } else if (strcmp(argv[3], "-ir") == 0) {
        tychecker c(tree);
        c.check();
        generator g;
        g.generate(tree, output_file, 1);
    } else if (strcmp(argv[3], "-ast") == 0) {
        std::ofstream outfile(output_file);
        std::streambuf* coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(outfile.rdbuf());
        print_exp(tree, 0);
        std::cout.rdbuf(coutbuf);
    } else if (strcmp(argv[3], "-ast+") == 0) {
        std::ofstream outfile(output_file);
        std::streambuf* coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(outfile.rdbuf());
        tychecker c(tree);
        c.check();
        print_exp(tree, 0);
        std::cout.rdbuf(coutbuf);
    } else {
        std::cout << "Unknown cmd arg. Expected args are:" << std::endl;
        std::cout << "-bc : generate llvm bytecode." << std::endl;
        std::cout << "-ir : generate llvm ir." << std::endl;
        std::cout << "-ast : generate abstract syntax tree." << std::endl;
        std::cout << "-ast+ : generate converted abstract syntax tree after semantic analyse." << std::endl;
    }
    return 0;
}