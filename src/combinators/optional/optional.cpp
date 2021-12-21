#include <combinators/optional/optional.h>

Optional::Optional(const Combinator& combinator)
                    : combinator_(combinator) {}

std::optional<Result> Optional::operator()(const std::vector<Token>& tokens, size_t pos) {
    auto result = combinator_(tokens, pos);
    if (result) {
        return result;
    }
    return Result{
        .value = {}, 
        .next_pos = pos
    };
}
