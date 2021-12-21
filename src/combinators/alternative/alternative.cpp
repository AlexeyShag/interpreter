#include <combinators/alternative/alternative.h>

Alternative::Alternative(const Combinator& left_combinator, 
                         const Combinator& right_combinator)
                            : left_combinator_(left_combinator)
                            , right_combinator_(right_combinator) {}

std::optional<Result> Alternative::operator()(const std::vector<Token>& tokens, size_t pos) {
    auto left_result = left_combinator_(tokens, pos);
    if (left_result) {
        return left_result;
    }
    auto right_result = right_combinator_(tokens, pos);
    if (right_result) {
        return right_result;
    }
    return std::nullopt;
}
