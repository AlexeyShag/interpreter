#include <combinators/lazy/lazy.h>

Lazy::Lazy(const Callback& callback) 
                : callback_(callback) {}

std::optional<Result> Lazy::operator()(const std::vector<Token>& tokens, size_t pos) {
    if (!combinator_) {
        combinator_ = callback_();
    }
    return combinator_(tokens, pos);
}