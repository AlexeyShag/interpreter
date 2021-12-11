#include <combinators/repeat/repeat.h>

Repeat::Repeat(const Combinator& combinator, 
                const Callback& unitor)
                    : combinator_(combinator)
                    , unitor_(unitor) {}

std::optional<Result> Repeat::operator()(const std::vector<Token>& tokens, size_t pos) {
    std::optional<Result> ans;
    ans = combinator_(tokens, pos);
    while (auto result = combinator_(tokens, ans.value().next_pos)) {
        ans = Result{
            .value = unitor_(ans.value().value, result.value().value),
            .next_pos = result.value().next_pos
        };
    }
    return ans;
}
