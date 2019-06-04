#pragma once
#include<string>
#include<iostream>
#include<memory>
#include "Instruction.h"

class Reader{
	static string exprs[];	//count,max,min,etc.
public:
    std::shared_ptr<Instruction> read(std::istream& = std::cin);
};
