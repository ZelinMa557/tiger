#include "lexer.h"
#include <iostream>
namespace FRONTEND {
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
            if(ch == '\n' || ch == EOF) {
                if(ch == '\n') line_cnt++;
                break;
            }
        }
    }
    // multi-line comment
    else if(ch == '*') {
        char last, cur;
        last = get_next_char();
        if(last == EOF) {
            std::cerr << "in line " << line_cnt << ":" <<std::endl;
            std::cerr<<"lexer: imcomplete comment, expected */" << std::endl;
            exit(1);
        }
        while(true) {
            cur = get_next_char();
            if(cur == EOF) {
                std::cerr << "in line " << line_cnt << ":" <<std::endl;
                std::cerr<<"lexer: imcomplete comment, expected */" << std::endl;
                exit(1);
            }
            if(last == '*' && cur == '/')
                break;
            last = cur;
        }
    }
    // maybe it's not a comment, it's div
    else {
        back_char();
        back_char();
        return;
    }
    ch = get_next_char();
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        if(ch == '\n') line_cnt++;
        ch = get_next_char();
    }
    if (ch == '/')
        escape_comment();
    else back_char();
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
        else if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            if(ch == '\n') line_cnt++;
            return ret;
        }
        else {
            ret += ch;
            std::cerr << "in line " << line_cnt << ":" <<std::endl;
            std::cerr << "lexer: invalid num " << ret <<"..."<<std::endl;
            exit(1);
        } 
    }
    // unreachable
    return {};
}

std::string lexer::get_identifier(char ch) {
    std::string ret;
    ret += ch;
    while(true) {
        ch = get_next_char();
        if(ch == EOF)
            return ret;
        else if(special.count(ch)) {
            back_char();
            return ret;
        }
        else if(ch == '/') {
            escape_comment();
            return ret;
        }
        else if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            if(ch == '\n') line_cnt++;
            return ret;
        }
        // valid char [0-9a-zA-Z]|_
        else if(ch >= '0' && ch <= '9' || ch == '_' || 
                ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
            ret += ch;
        }
        else {
            std::cerr << "in line " << line_cnt << ":" <<std::endl;
            std::cerr << "lexer: char " << ch <<" are not expected to occur in identifier."<<std::endl;
            exit(1);
        } 
    }
    // unreachable
    return {};
}

std::string lexer::get_operator(char ch) {
    // only operator start with [:<>] may contain more than one chars.
    if(ch != ':' && ch != '<' && ch != '>') {
        return {ch};
    }
    char next = get_next_char();
    if(ch == ':' && next == '=' || ch == '<' && next == '>'
        || ch == '<' && next == '=' || ch == '>' && next == '=') {
        std::string ret{ch};
        return ret+next;
    }
    back_char();
    return {ch};
}

std::string lexer::get_string() {
    char last = '"', ch;
    std::string ret;
    ret += '"';
    while(true) {
        ch = get_next_char();
        if(ch == EOF) {
            std::cerr << "in line " << line_cnt << ":" <<std::endl;
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
        if(ch == '\n') line_cnt++;
        ch = get_next_char();
        if(ch == EOF) return {};
    }
    std::string ret;
    if(ch == '/') {
        escape_comment();
        ch = get_next_char();
    }
    // escape space after escape comment
    while (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
        if(ch == '\n') line_cnt++;
        ch = get_next_char();
        if(ch == EOF) return {};
    }
    if(ch == EOF) return {};

    // parse string
    if(ch == '"') 
        return get_string();
    // parse number
    else if(ch >= '0' && ch <= '9') 
        return get_num(ch);
    // parse operator
    else if(special.count(ch))
        return get_operator(ch);
    // parse identifier or other reserved word.
    else
        return get_identifier(ch);
}

token lexer::next() {
    std::string next_ = next_word();
    if(convert.count(next_))
        return {convert[next_], line_cnt};
    if(next_.length() == 0)
        return {EOF, line_cnt};
    if('0' <= next_[0] && next_[0] <= '9')
        return {INT_LITERAL, next_, line_cnt};
    if(next_[0] == '"')
        return {STR_LITERAL, next_, line_cnt};
    return {IDENTIFIER, next_, line_cnt};
}
}