#pragma once

#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>
#include <combinators/lazy/lazy.h>
#include <combinators/phrase/phrase.h>
#include <combinators/reserved/reserved.h>
#include <combinators/separator/separator.h>
#include <combinators/processor/processor.h>
#include <combinators/optional/optional.h>
#include <combinators/repeat/repeat.h>
#include <combinators/tag/tag.h>

#include <types/result.h>
#include <types/token.h>

namespace languge {

std::shared_ptr<Statement> Parse(const std::vector<Token>&);

};