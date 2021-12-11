#pragma once

#include <optional>
#include <types/token.h>
#include <types/result.h>

class Concator;
class Alternative;
class Processor;

class BaseCombinator {
public:
    virtual std::optional<Result> operator()(const std::vector<Token>&, size_t) = 0;
    virtual ~BaseCombinator() {}
private:
};


