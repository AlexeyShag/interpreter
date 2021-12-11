#pragma once

#include <optional>

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>

class Phrase: public BaseCombinator {
public:
    Phrase(const Combinator&);
    std::optional<Result> operator()(const std::vector<Token>&, size_t) override;
    
private:
    Combinator combinator_;
};