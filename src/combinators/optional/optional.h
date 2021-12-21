#pragma once 

#include <optional>

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>

class Optional: public BaseCombinator {
public:
    Optional(const Combinator&);
    std::optional<Result> operator()(const std::vector<Token>&, size_t) override;
    
private:
    Combinator combinator_;
};