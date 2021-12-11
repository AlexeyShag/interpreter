#pragma once

#include <optional>

#include <combinators/base_combinator/base_combinator.h>

class Reserved: public BaseCombinator {
public:
    Reserved(const std::string&, TokenClass);
    std::optional<Result> operator()(const std::vector<Token>&, size_t) override;

private:
    std::string value_;
    TokenClass token_class_;
};