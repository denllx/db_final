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

	inline std::string toregex(std::string src) {
		std::string dst;
		for (int i = 0, len = src.size(); i < len; i++) {
			if (src[i] == '%') {
				dst += ".*?";
			}
			else dst.push_back(src[i]);
		}
		return dst;
	}
    
}