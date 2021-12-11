#include <combinators/processor/processor.h>
#include <iostream>

Processor::Processor(Combinator combinator, const Callback& callback)
                            : combinator_(combinator)
                            , callback_(callback) {}

std::optional<Result> Processor::operator()(const std::vector<Token>& tokens, size_t pos) {
    auto result = combinator_(tokens, pos);
    if (result) {
        result.value().value = callback_(result.value().value);
    }
    return result;
}
