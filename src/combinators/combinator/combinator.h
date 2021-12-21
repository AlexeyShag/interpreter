#pragma once

#include <optional>
#include <functional>

#include <combinators/base_combinator/base_combinator.h>

class Combinator {
public:
    using Callback = std::function<Result::Value(const Result::Value&)>;

    Combinator() = default;

    Combinator(BaseCombinator*);

    template<typename T>
    Combinator(std::shared_ptr<T> combinator) {
        combinator_ = std::dynamic_pointer_cast<BaseCombinator>(combinator);
    }

    std::optional<Result> operator()(const std::vector<Token>&, size_t);

    operator bool();

    Combinator operator+(const Combinator& combinator);
    Combinator operator|(const Combinator& combinator);    
    Combinator operator^(const Callback& callback);

private:
    std::shared_ptr<BaseCombinator> combinator_;
};