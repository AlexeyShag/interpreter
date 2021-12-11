#pragma once 

#include <optional>

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>

class Separator: public BaseCombinator {
public:
    using Callback = std::function<Result::Value(const Result::Value&, const Result::Value&)>;
    Separator(const Combinator&, const Combinator&, const Callback&);
    std::optional<Result> operator()(const std::vector<Token>&, size_t);

private:
    Combinator combinator_;
    Combinator separator_;
    Callback unitor_;
};