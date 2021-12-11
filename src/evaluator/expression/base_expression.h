#pragma once

#include <unordered_map>

#include <types/default_type.h>

class Expression {
public:
    virtual DefaultType eval(std::unordered_map<std::string, DefaultType>&) = 0;
    virtual ~Expression() {}
};