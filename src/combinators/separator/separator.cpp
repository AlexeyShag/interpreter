#include <combinators/separator/separator.h>
#include <iostream>

Separator::Separator(const Combinator& combinator, const Combinator& separator, const Callback& unitor) 
                : combinator_(combinator)
                , separator_(separator)
                , unitor_(unitor) {}

std::optional<Result> Separator::operator()(const std::vector<Token>& tokens_, size_t pos) {
    auto result = combinator_(tokens_, pos);
    auto next_combinator = separator_ + combinator_;
    while (auto next_result = next_combinator(tokens_, result.value().next_pos)) {
        result = Result{
           .value = unitor_(result.value().value, next_result.value().value),
           .next_pos = next_result.value().next_pos
        };
    }
    return result;
}