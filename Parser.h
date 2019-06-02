#pragma once
#include<string>
#include<sstream>

class Parser{
    std::istringstream ss;
    std::string head;
    bool is_end;

public:
    Parser()=default;
    Parser(const std::string &);
    static bool is_oper(char);
    void reset(const std::string &);
    std::string lookahead();
    bool lookahead(const std::string&);
    bool ended();
    void skip(int cnt=1);
    void match_token(const std::string &);
    std::string get_str();
};
