#pragma once

#include <unordered_map>
#include <string>

#include <types/default_type.h>

class Statement {
public:
    virtual void eval(std::unordered_map<std::string, DefaultType>&) = 0;
    virtual ~Statement() {}
};