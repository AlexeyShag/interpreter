#pragma once 

#include <optional>

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>

class Processor: public BaseCombinator {
public:
    using Callback = std::function<Result::Value(const Result::Value&)>;
    Processor(Combinator, const Callback&);
    std::optional<Result> operator()(const std::vector<Token>&, size_t) override;
    
private:
    Combinator combinator_;
    Callback callback_;
};