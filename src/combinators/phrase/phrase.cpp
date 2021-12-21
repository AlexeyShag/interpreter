#include <combinators/phrase/phrase.h>
#include <iostream>

Phrase::Phrase(const Combinator& combinator)
                            : combinator_(combinator) {}

std::optional<Result> Phrase::operator()(const std::vector<Token>& tokens, size_t pos) {
    auto result = combinator_(tokens, pos);
    if (result && tokens.size() == result.value().next_pos) {
        return result;
    }
    return std::nullopt;
    
}
