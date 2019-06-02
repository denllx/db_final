#pragma once
#include<algorithm>
#include<string>
#include<utility>

namespace utils{
    inline std::string tolower(std::string str){
	    std::transform(str.begin(), str.end(),str.begin(),::tolower);
	    return str;
	}
	
	inline std::string toupper(std::string str){
	    std::transform(str.begin(), str.end(),str.begin(),::toupper);
	    return str;
	}
    
}