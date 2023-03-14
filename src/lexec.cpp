#include "lexer.h"
#include <iostream>
lexer::lexer(std::string src_file) {
    src.open(src_file, std::ios::in);
    if(!src.is_open()) {
        std::cerr << "Fail to open file " << src_file << std::endl;
        exit(1);
    }
}

lexer::~lexer() {
    src.close();
}

void lexer::escape_comment() {
    // escape comment
    char ch = get_next_char();
    // single line comment
    if(ch == '/') {
        while(true) {
            ch = get_next_char();
            if(ch == '\n' || ch == 'EOF')
                break;
        }
    }
    // multi-line comment
    else if(ch == '*') {
        char last, cur;
        last = get_next_char();
        if(last == EOF) {
            std::cerr<<"lexer: imcomplete comment, expected */" << std::endl;
            exit(1);
        }
        while(true) {
            cur = get_next_char();
            if(cur == EOF) {
                std::cerr<<"lexer: imcomplete comment, expected */" << std::endl;
                exit(1);
            }
            if(last == '*' && cur == '/')
                break;
        }
    }
    else {
        std::cerr<<"lexer: unknown comment type /"<< ch <<", expected // or /* ." << std::endl;
        exit(1);
    }
}
std::string lexer::next_word() {
    char ch = get_next_char();
    if(ch == EOF) return {};
    // escape space
    while (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
        ch = get_next_char();
        if(ch == EOF) return {};
    }
    std::string ret;
    if(ch == '/') {
        escape_comment();
        ch = get_next_char();
    }
    // parse string
    if(ch == '"') {
        char last = ch;
        ret += '"';
        while(true) {
            ch = get_next_char();
            if(ch == EOF) {
                std::cerr << "lexer: fail to get complete string." << std::endl;
                exit(1);
            }
            if(ch != '"') {
                ret += ch;
            } else if(last != '/') {
                break;
            }
            last = ch;
        }
        ret += '"';
        return ret;
    }
}