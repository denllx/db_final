#pragma once
#include<string>
#include<iostream>
#include<memory>
#include "Instruction.h"

class Reader{
public:
    std::shared_ptr<Instruction> read(std::istream& = std::cin);
};
