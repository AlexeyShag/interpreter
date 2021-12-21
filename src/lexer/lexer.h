#pragma once

#include <vector>
#include <finite_state_machine/finite_state_machine.h>

#include <types/token.h>

class Lexer {
public:
    Lexer();
    void AddMachine(const FiniteStateMachine&, TokenClass);
    std::vector<Token> SplitText(const std::string& text);

private:
    std::vector<std::pair<FiniteStateMachine, TokenClass>> machines_; 
};