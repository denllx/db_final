#include <string>
#include <sstream>
#include <cassert>
#include <utility>
#include <iostream>
#include "Parser.h"
#include "utils.h"
using namespace utils;
using namespace std;
#define pb push_back

Parser::Parser(const string &s):Parser(){
    reset(s);
}

bool Parser::is_oper(char c){
    static const char str[]="()*,=<>";
    for(const char *s=str;*s;++s){
        if(*s==c) return true;
    }
    return false;
}

void Parser::reset(const string &s){
    string tmp;
    bool in_quotation=false;
    for(char c:s){
        if(c=='\"'||c=='\'')
            in_quotation^=1;
        if(!in_quotation&&is_oper(c)){
            tmp.pb(' ');
            tmp.pb(c);
            tmp.pb(' ');
        }else{
            tmp.pb(c);
        }
    }
    ss.str(tmp);
    ss.clear();
    is_end=false;
    skip();
}

void Parser::skip(int cnt){
    for(;cnt>0;--cnt){
        if(is_end) return;
        if(!(ss>>head)){
            head="#";
            is_end=true;
            return;
        }
    }
}

bool Parser::ended(){
    return is_end;
}

string Parser::lookahead(){
    return head;
}

bool Parser::lookahead(const string &s){
    return tolower(s)==tolower(head);
}

void Parser::match_token(const string &s){
    assert(tolower(s)==tolower(head));
    skip();
}

string Parser::get_str(){
    if(is_end) return "#";
    string tmp=move(head);
    skip();
    return tmp;
}