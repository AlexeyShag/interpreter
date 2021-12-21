#pragma once 

#include <string>

enum class TokenClass {
    kNone = 0,
    kReserved = 1,
    kInt = 2,
    kStr = 3,
    kId = 4,
    kBool = 5
};

struct Token {
    std::string text;
    TokenClass token_class;
};
