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

std::string lexer::get_num(char ch) {
    std::string ret;
    ret += ch;
    while(true) {
        ch = get_next_char();
        if(ch >= '0' && ch <= '9')
            ret += ch;
        else if(ch == EOF)
            return ret;
        else if(special.count(ch)) {
            back_char();
            return ret;
        }
        else if(ch == '/') {
            escape_comment();
            return ret;
        }
        else if(ch == ' ' || ch == '\t' || ch == '\n')
            return ret;
        else {
            ret += ch;
            std::cerr << "lexer: invalid identifier " << ret <<"..."<<std::endl;
        } 
    }
}

std::string lexer::get_string() {
    char last = '"', ch;
    std::string ret;
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
    if(ch == EOF) return {};
    // parse string
    if(ch == '"') 
        return get_string();
    // parse number
    else if(ch >= '0' && ch <= '9') 
        return get_num(ch);
    else if(special.count(ch))
        return get_operator(ch);
    else
        return get_identifier(ch);
}