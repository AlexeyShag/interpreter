#include <gtest/gtest.h>
#include <combinators/base_combinator/base_combinator.h>
#include <combinators/combinator/combinator.h>
#include <combinators/lazy/lazy.h>
#include <combinators/phrase/phrase.h>
#include <combinators/reserved/reserved.h>
#include <combinators/separator/separator.h>
#include <combinators/processor/processor.h>
#include <combinators/optional/optional.h>
#include <combinators/alternative/alternative.h>
#include <combinators/concator/concator.h>
#include <combinators/repeat/repeat.h>
#include <combinators/tag/tag.h>

#include <types/token.h>
#include <types/result.h>

#include "helper.h"

TEST(Combinators, test_tag) {
    Tag tag(TokenClass::kId);
    std::vector<Token> tokens;
    std::vector<bool> answers;
    for (size_t i = 0; i < 10; ++i) {
        if (GetRandomNumber(0, 1) == 0) {
            tokens.push_back(Token{
                .text = "test",
                .token_class = TokenClass::kInt
            });
            answers.push_back(false);
        } else {
            tokens.push_back(Token{
                .text = "test",
                .token_class = TokenClass::kId
            });
            answers.push_back(true);
        }
    }

    for (size_t i = 0; i < 10; ++i) {
        auto result = tag(tokens, i);
        if (answers[i]) {
            EXPECT_TRUE(result);
        } else {
            EXPECT_FALSE(result);
        }
    }
}

TEST(Combinators, test_reserved) {
    Reserved reserved("test", TokenClass::kReserved);
    std::vector<Token> tokens;
    std::vector<bool> answers;
    for (size_t i = 0; i < 10; ++i) {
        if (GetRandomNumber(0, 1) == 0) {
            tokens.push_back(Token{
                .text = "test",
                .token_class = TokenClass::kReserved
            });
            answers.push_back(true);
        } else {
            tokens.push_back(Token{
                .text = "test1",
                .token_class = TokenClass::kId
            });
            answers.push_back(false);
        }
    }

    for (size_t i = 0; i < 10; ++i) {
        auto result = reserved(tokens, i);
        if (answers[i]) {
            EXPECT_TRUE(result);
        } else {
            EXPECT_FALSE(result);
        }
    }
}

TEST(Combinators, test_alternative) {
    Combinator reserved1(std::make_shared<Reserved>("test", TokenClass::kReserved));
    Combinator reserved2(std::make_shared<Reserved>("test1", TokenClass::kReserved));
    Alternative alternative(reserved1, reserved2);
    std::vector<Token> tokens;
    std::vector<bool> answers;
    for (size_t i = 0; i < 100; ++i) {
        if (GetRandomNumber(0, 2) == 0) {
            tokens.push_back(Token{
                .text = "test",
                .token_class = TokenClass::kReserved
            });
            answers.push_back(true);
        } else if (GetRandomNumber(0, 2) == 1) {
            tokens.push_back(Token{
                .text = "test1",
                .token_class = TokenClass::kReserved
            });
            answers.push_back(true);
        } else {
            tokens.push_back(Token{
                .text = "test2",
                .token_class = TokenClass::kId
            });
            answers.push_back(false);
        }
    }

    for (size_t i = 0; i < 100; ++i) {
        auto result = alternative(tokens, i);
        if (answers[i]) {
            EXPECT_TRUE(result);
        } else {
            EXPECT_FALSE(result);
        }
    }
}

TEST(Combinators, test_concat) {
    Combinator reserved1(std::make_shared<Reserved>("test", TokenClass::kReserved));
    Combinator reserved2(std::make_shared<Reserved>("test1", TokenClass::kReserved));
    Concator concator(reserved1, reserved2);
    std::vector<Token> tokens;
    std::vector<int> answers;
    for (size_t i = 0; i < 100; ++i) {
        if (GetRandomNumber(0, 2) == 0) {
            tokens.push_back(Token{
                .text = "test",
                .token_class = TokenClass::kReserved
            });
            answers.push_back(1);
        } else if (GetRandomNumber(0, 2) == 1) {
            tokens.push_back(Token{
                .text = "test1",
                .token_class = TokenClass::kReserved
            });
            answers.push_back(2);
        } else {
            tokens.push_back(Token{
                .text = "test2",
                .token_class = TokenClass::kId
            });
            answers.push_back(3);
        }
    }

    for (size_t i = 0; i < 99; ++i) {
        auto result = concator(tokens, i);
        if (answers[i] == 1 && answers[i + 1] == 2) {
            EXPECT_TRUE(result);
        } else {
            EXPECT_FALSE(result);
        }
    }
}


TEST(Combinators, test_optional) {
    Combinator reserved1(std::make_shared<Reserved>("test", TokenClass::kReserved));
    Optional optional(reserved1);
    std::vector<Token> tokens;
    std::vector<int> answers;
    for (size_t i = 0; i < 10; ++i) {
        if (GetRandomNumber(0, 2) == 0) {
            tokens.push_back(Token{
                .text = "test",
                .token_class = TokenClass::kReserved
            });
            answers.push_back(1);
        } else if (GetRandomNumber(0, 2) == 1) {
            tokens.push_back(Token{
                .text = "test1",
                .token_class = TokenClass::kReserved
            });
            answers.push_back(2);
        } else {
            tokens.push_back(Token{
                .text = "test2",
                .token_class = TokenClass::kId
            });
            answers.push_back(3);
        }
    }

    for (size_t i = 0; i < 9; ++i) {
        auto result = optional(tokens, i);
        if (answers[i] == 1) {
            EXPECT_EQ(result.value().next_pos, i + 1);
        } else {
            EXPECT_EQ(result.value().next_pos, i);
        }
    }
}


TEST(Combinators, test_processor) {
    Combinator reserved1(std::make_shared<Tag>(TokenClass::kReserved));
    auto Action = [](const Result::Value& result) {
        std::string text = std::any_cast<std::string>(result[0]);
        std::reverse(text.begin(), text.end());
        return Result::Value{text};
    };
    Processor processor(reserved1, Action);
    std::vector<Token> tokens;
    std::vector<std::string> answers;
    for (size_t i = 0; i < 10; ++i) {
        std::string s = GetRandomString(10, {'a', 'b', 'c'});
        tokens.push_back(Token{
            .text = s,
            .token_class = TokenClass::kReserved
        });
        std::reverse(s.begin(), s.end());
        answers.push_back(s);
    }

    for (size_t i = 0; i < 9; ++i) {
        auto result = processor(tokens, i);
        std::string s = std::any_cast<std::string>(result.value().value[0]);
        EXPECT_EQ(s, answers[i]);
    }
}

/*
TEST(Combinators, test_repeat) {
    Combinator reserved1(std::make_shared<Tag>(TokenClass::kReserved));
    auto Action = [](const Result::Value& result) {
        std::string text = std::any_cast<std::string>(result[0]);
        std::reverse(text.begin(), text.end());
        return Result::Value{text};
    };
    Repeat repeat(reserved1, Action);
    std::vector<Token> tokens;
    std::vector<std::string> answers;
    for (size_t i = 0; i < 10; ++i) {
        std::string s = GetRandomString(10, {'a', 'b', 'c'});
        tokens.push_back(Token{
            .text = s,
            .token_class = TokenClass::kReserved
        });
        std::reverse(s.begin(), s.end());
        answers.push_back(s);
    }

    for (size_t i = 0; i < 9; ++i) {
        auto result = processor(tokens, i);
        std::string s = std::any_cast<std::string>(result.value().value[0]);
        EXPECT_EQ(s, answers[i]);
    }
}*/