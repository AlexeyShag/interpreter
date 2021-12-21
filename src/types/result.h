#pragma once 

#include <vector>
#include <memory>
#include <any>

#include <evaluator/expression/expression.h>
#include <evaluator/statement/statement.h>

struct Result {
    using Value = std::vector<std::any>;
    template<typename Derive>
    static Value MakeValue(const std::shared_ptr<Derive>& ptr) {
        if constexpr (std::is_base_of_v<Statement, Derive>) {
            return Value{std::dynamic_pointer_cast<Statement>(ptr)};
        }
        if constexpr (std::is_base_of_v<Expression, Derive>) {
            return Value{std::dynamic_pointer_cast<Expression>(ptr)};
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