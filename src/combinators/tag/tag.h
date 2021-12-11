#pragma once

#include <combinators/base_combinator/base_combinator.h>

class Tag: public BaseCombinator {
public:
    Tag(TokenClass);
    std::optional<Result> operator()(const std::vector<Token>&, size_t) override;

private:
    TokenClass token_class_;
};