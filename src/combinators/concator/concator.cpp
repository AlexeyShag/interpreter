#include <combinators/concator/concator.h>

Concator::Concator(const Combinator& left_combinator, 
                   const Combinator& right_combinator)
                            : left_combinator_(left_combinator)
                            , right_combinator_(right_combinator) {}

std::optional<Result> Concator::operator()(const std::vector<Token>& tokens, size_t pos) {
    auto left_result = left_combinator_(tokens, pos);
    if (!left_result) {
        return std::nullopt;
    }
    auto right_result = right_combinator_(tokens, left_result.value().next_pos);
    if (!right_result) {
        return std::nullopt;
    }
    return Result{
        .value = left_result.value().value + right_result.value().value,
        .next_pos = right_result.value().next_pos
    };
}
