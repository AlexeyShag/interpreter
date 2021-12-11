#pragma once

#include <any>
#include <vector>
#include <iostream>
#include <evaluators/base_evaluator/init.h>

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

///using DefaultType = std::variant<int, std::string, bool>;

struct Result {
    using Value = std::vector<std::any>;
    template<typename Derive>
    static Value MakeValue(const std::shared_ptr<Derive>& ptr) {
        if constexpr (std::is_base_of_v<Statement, Derive>) {
            std::cerr << "Statement" << '\n';
            return Value{std::dynamic_pointer_cast<Statement>(ptr)};
        }
        if constexpr (std::is_base_of_v<Expression, Derive>) {
            return Value{std::dynamic_pointer_cast<Expression>(ptr)};
        }
        if constexpr (std::is_base_of_v<BooleanExpression, Derive>) {
            return Value{std::dynamic_pointer_cast<BooleanExpression>(ptr)};
        }
    }

    static Value MakeValue(const std::string& text) {
        return Value{text};
    }
    
    Value value;
    size_t next_pos = 0;
};

template<typename T> 
std::vector<T> operator+(const std::vector<T> first, const std::vector<T> second) {
    std::vector<T> ans = first;
    ans.insert(ans.end(), second.begin(), second.end());
    return ans;
}