#pragma once 

#include <optional>

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>

class Repeat: public BaseCombinator {
public:
    using Callback = std::function<Result::Value(const Result::Value&, const Result::Value&)>;
    Repeat(const Combinator&, const Callback&);
    std::optional<Result> operator()(const std::vector<Token>&, size_t) override;

private:
    Combinator combinator_;
    Callback unitor_;
};
