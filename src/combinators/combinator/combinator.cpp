
#include <combinators/combinator/combinator.h>
#include <combinators/alternative/alternative.h>
#include <combinators/concator/concator.h>
#include <combinators/processor/processor.h>

Combinator::Combinator(BaseCombinator* combinator) 
                        : combinator_(combinator) {}

std::optional<Result> Combinator::operator()(const std::vector<Token>& tokens, size_t pos) {
    return (*combinator_)(tokens, pos);
}

Combinator::operator bool() {
    return combinator_ != nullptr;
}

Combinator Combinator::operator+(const Combinator& combinator) {
    return std::make_shared<Concator>(*this, combinator);
}

Combinator Combinator::operator|(const Combinator& combinator) {
    return std::make_shared<Alternative>(*this, combinator);
}

Combinator Combinator::operator^(const Callback& callback) {
    return std::make_shared<Processor>(*this, callback);
}