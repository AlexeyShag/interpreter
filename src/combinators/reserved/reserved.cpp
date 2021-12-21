#include <combinators/reserved/reserved.h>
#include <iostream>

Reserved::Reserved(const std::string& value, TokenClass token_class) 
                    : value_(value)
                    , token_class_(token_class) {}


std::optional<Result> Reserved::operator()(const std::vector<Token>& tokens, size_t pos) {
    if (pos < tokens.size() 
            && tokens[pos].text == value_ 
            && tokens[pos].token_class == token_class_) {
        return Result{
            .value = Result::Value{tokens[pos].text}, 
            .next_pos = pos + 1
        };
    } else {
        return std::nullopt;
    }
}