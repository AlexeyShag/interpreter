#pragma once 

#include <optional>

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>

class Alternative: public BaseCombinator {
public:
    Alternative(const Combinator&, const Combinator&);
    std::optional<Result> operator()(const std::vector<Token>&, size_t);
private:
    Combinator left_combinator_;
    Combinator right_combinator_;
};