#include <combinators/tag/tag.h>
#include <iostream>

Tag::Tag(TokenClass token_class) 
                    : token_class_(token_class) {}


std::optional<Result> Tag::operator()(const std::vector<Token>& tokens, size_t pos) {
    if (pos < tokens.size() 
            && tokens[pos].token_class == token_class_) {
        return Result{
            .value = Result::Value{tokens[pos].text}, 
            .next_pos = pos + 1
        };
    } else {
        return std::nullopt;
    }
}