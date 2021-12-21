#pragma once

#include <vector>
#include <finite_state_machine/finite_state_machine.h>

#include <types/token.h>

namespace languge {

FiniteStateMachine GetReservedParser() {
    FiniteStateMachine machine;

    machine.SetNumberState(2).SetInitialState(0);

    machine.AddString(";");
    machine.AddString(",");
    machine.AddString(":=");
    machine.AddString("(");
    machine.AddString(")");

    machine.AddString("+");
    machine.AddString("-");
    machine.AddString("*");
    machine.AddString("/");

    machine.AddString("and");
    machine.AddString("or");
    machine.AddString("xor");
    machine.AddString("not");

    machine.AddString("cast_to_int");
    machine.AddString("cast_to_bool");
    machine.AddString("cast_to_str");
    machine.AddString("get_type");

    machine.AddString("<");
    machine.AddString(">");
    machine.AddString("<=");
    machine.AddString(">=");
    machine.AddString("<>");
    machine.AddString("=");

    machine.AddString("if");
    machine.AddString("then");
    machine.AddString("while");
    machine.AddString("do");
    machine.AddString("begin");
    machine.AddString("end");
    machine.AddString("else");
    machine.AddString("to");
    machine.AddString("downto");
    machine.AddString("for");

    machine.AddString("read");
    machine.AddString("write");
    machine.AddString("writeln");

    return machine;
} 

FiniteStateMachine GetVariableParser() {
    FiniteStateMachine machine;
    machine.SetNumberState(1)
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({0}));
    
    for (char i = 'a'; i <= 'z'; ++i) {
        machine.AddTransition(0, i, 0);
    }

    for (char i = 'A'; i <= 'Z'; ++i) {
        machine.AddTransition(0, i, 0);
    }
    machine.AddTransition(0, '_', 0);

    return machine;
}

FiniteStateMachine GetIntParser() {
    FiniteStateMachine machine;
    machine.SetNumberState(1)
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({0}));

    for (char i = '0'; i <= '9'; ++i) {
        machine.AddTransition(0, i, 0);
    }

    return machine;
}

FiniteStateMachine GetStrParser() {
    FiniteStateMachine machine;
    machine.SetNumberState(3)
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({2}))
        .AddTransition(0, '\"', 1)
        .AddTransition(1, '\"', 2);

    for (char i = ' '; i < char(127); ++i) {
        if (i == '\"') {
            continue;
        }
        machine.AddTransition(1, char(i), 1);
    }
    
    return machine;
}

FiniteStateMachine GetBoolParser() {
    FiniteStateMachine machine;
    machine.SetNumberState(1).SetInitialState(0);

    machine.AddString("true");
    machine.AddString("false");
    
    return machine;
}

FiniteStateMachine GetNoneParser() {
    FiniteStateMachine machine;
    machine.SetNumberState(3)
        .AddAlpabet(std::vector<char>({' ', '\n', '\t'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({0, 2}))
        .AddTransition(0, ' ', 0)
        .AddTransition(0, '\t', 0)
        .AddTransition(0, '\n', 0);

    machine.AddTransition(0, '/', 1)
            .AddTransition(1, '/', 2);
    
    for (int i = ' '; i < 128; ++i) {
        machine.AddTransition(2, char(i), 2);
    }

    return machine;        
}

Lexer GetLexer() {
    Lexer lexer;
    lexer.AddMachine(GetReservedParser(), TokenClass::kReserved);
    lexer.AddMachine(GetIntParser(), TokenClass::kInt);
    lexer.AddMachine(GetStrParser(), TokenClass::kStr);
    lexer.AddMachine(GetBoolParser(), TokenClass::kBool);
    lexer.AddMachine(GetVariableParser(), TokenClass::kId);
    lexer.AddMachine(GetNoneParser(), TokenClass::kNone);

    return lexer;
}

}