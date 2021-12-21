#include <lexer/lexer.h>
#include <string_view>
#include <string>

Lexer::Lexer() {}

void Lexer::AddMachine(const FiniteStateMachine& machine, TokenClass token_class) {
    machines_.push_back(std::make_pair(machine, token_class));
}

std::vector<Token> Lexer::SplitText(const std::string& text) {
    size_t pos = 0;
    std::vector<Token> tokens;
    while (pos < text.size()) {
        size_t length = 0;
        for (const auto& [machine, token_class]: machines_) {
            length = machine.MatchPrefix(std::string(text.begin() + pos, text.end()));
            if (length) {
                if (token_class != TokenClass::kNone) {
                    tokens.push_back(Token{
                            .text = std::string(text.begin() + pos, text.begin() + pos + length),
                            .token_class = token_class
                    });
                }
                pos += length;
                break;
            }
        }
        if (!length) {
            throw std::runtime_error("invalid syntax");
        }
    }
    return tokens;
}
