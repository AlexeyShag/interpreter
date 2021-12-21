#pragma once

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>

class Lazy: public BaseCombinator {
public:
    using Callback = std::function<Combinator()>;

    Lazy(const Callback&);
    std::optional<Result> operator()(const std::vector<Token>&, size_t)override;

private:
    Callback callback_;
    Combinator combinator_;
};
