#include "lexer.h"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    if(argc < 2) {
        cerr << "need filename!" <<endl;
    }
    string file_name(argv[1]);
    lexer l(file_name);
    while(true) {
        token t = l.next();
        if(t.type == EOF)
            break;
        cout << t.to_str() << endl;
    }
    return 0;
}