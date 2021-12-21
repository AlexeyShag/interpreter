#include <languge/parser.h>
#include <evaluator/expression/expression.h>
#include <evaluator/statement/statement.h>

#include <iostream>

namespace languge {

static std::vector<std::vector<std::string>> operators = { 
    {"*", "/"}, 
    {"+", "-"},
    {"<", "<=", "=", "<>", ">", ">="},
    {"and", "xor", "or"} 
};

Combinator ProcessTerm();
Combinator ProcessGroup();
Combinator ExpressionStatements();
Combinator AssignStatements();
Combinator Statements();
Combinator StatementsList();
Combinator Expression();
Combinator WriteStatement();
Combinator ReadStatement();
Combinator IfStatement();
Combinator WhileStatement();
Combinator UnaryOperation();
Combinator ForStatement();

std::shared_ptr<Statement> Parse(const std::vector<Token>& result) {
    auto parser_result = Phrase(Statements())(result, 0).value();
    Result::Value value = parser_result.value;
    if (value.size() != 1) {
        throw std::runtime_error("syntax error");
    }
    return std::any_cast<std::shared_ptr<Statement>>(value[0]);
}

Combinator GetOptional(const Combinator& combinator) {
    return std::make_shared<Optional>(combinator);
}

template <typename Callback>
Combinator GetLazy(Callback func) {
    return std::make_shared<Lazy>(func);
}

Combinator KeyWord (const std::string& name) {
    return std::make_shared<Reserved>(name, TokenClass::kReserved);
}

Combinator Id () {
    return std::make_shared<Tag>(TokenClass::kId);
}

Combinator Num() {
    auto GetNum = [](const Result::Value& value) {
        return value;
    };
    return Combinator(std::make_shared<Tag>(TokenClass::kInt)) ^ GetNum;
}

Combinator Str() {
    auto GetStr = [](const Result::Value& value) {
        auto str = std::any_cast<std::string>(value[0]);
        return Result::Value{std::string(str.begin() + 1, str.end() - 1)};
    };
    return Combinator(std::make_shared<Tag>(TokenClass::kStr)) ^ GetStr;
}

Combinator Bool() {
    auto GetBool = [](const Result::Value& value) {
        return value;
    };
    return Combinator(std::make_shared<Tag>(TokenClass::kBool)) ^ GetBool;
}

Combinator Precedence(const Combinator& combinator, 
                        const std::vector<std::vector<std::string>>& operators,
                        const std::function<Result::Value(const Result::Value&, const Result::Value&)> unitor) {

    Combinator new_combinator = combinator;
    for (const auto& group_operators: operators) {
        Combinator operators_combinator = KeyWord(group_operators[0]);
        for (size_t i = 1; i < group_operators.size(); ++i) {
            operators_combinator = operators_combinator | KeyWord(group_operators[i]);
        }
        new_combinator = std::make_shared<Separator>(new_combinator, operators_combinator, unitor);
    }
    return new_combinator;
}

Combinator Statements() {
    auto Unite = [](Result::Value left, Result::Value right) {
        auto left_statement = std::any_cast<std::shared_ptr<Statement>>(left[0]);
        auto right_statement = std::any_cast<std::shared_ptr<Statement>>(right[0]);
        return Result::MakeValue(std::make_shared<CompoundStatement>(left_statement, right_statement));
    };
    return std::make_shared<Repeat>(StatementsList(), Unite);
}

Combinator StatementsList() {
    return AssignStatements() |
            WriteStatement() |
            ReadStatement() |
            IfStatement() |
            WhileStatement() |
            ForStatement();
}

Combinator AssignStatements() {
    auto GetStatement = [](Result::Value value) {    
        auto variable = std::any_cast<std::string>(value[0]);
        auto expresion = std::any_cast<std::shared_ptr<class Expression>>(value[2]);
        return Result::MakeValue(std::make_shared<AssignStatement>(variable, expresion));
    };
    return Id() + KeyWord(":=") + Expression() + KeyWord(";") ^ GetStatement;
}



Combinator Expression() {
    auto Unitor = [](const Result::Value& left, const Result::Value& right) {
        auto left_expresion = std::any_cast<std::shared_ptr<class Expression>>(left[0]);
        auto op = std::any_cast<std::string>(right[0]);
        auto right_expresion = std::any_cast<std::shared_ptr<class Expression>>(right[1]);
        return Result::MakeValue(std::make_shared<BinaryOperationExpression>(op, left_expresion, right_expresion));
    };
    return Precedence(ExpressionStatements(), operators, Unitor);
}

Combinator ExpressionStatements() {
    return ProcessGroup() | ProcessTerm() | UnaryOperation();
}

Combinator ProcessGroup() {
    auto GetMiddle = [](const Result::Value& result) {
        return Result::Value{result[1]};
    };
    return KeyWord("(") + GetLazy(Expression) + KeyWord(")") ^ GetMiddle;
}

Combinator ProcessTerm() {
    auto CreateConstExpression = [](Result::Value result) {
        auto text = std::any_cast<std::string>(result[0]);
        int val = 0;
        for (char c: text) {
            val *= 10;
            val += c - '0';
        }
        return Result::MakeValue(std::make_shared<ConstExpression>(val));
    };
    auto CreateVariableExpression = [](Result::Value result) {
        auto text = std::any_cast<std::string>(result[0]);
        return Result::MakeValue(std::make_shared<VariableExpression>(text));
    };
    auto CreateVariableStringExpression = [](Result::Value result) {
        auto text = std::any_cast<std::string>(result[0]);
        return Result::MakeValue(std::make_shared<ConstExpression>(text));
    };
    auto CreateBooleanStringExpression = [](Result::Value result) {
        auto text = std::any_cast<std::string>(result[0]);
        bool flag = false;
        if (text == "true") {
            flag = true;
        } else if (text == "false") {
            flag = false;
        } else {
            throw std::runtime_error("invalid bool");
        }
        return Result::MakeValue(std::make_shared<ConstExpression>(flag));
    };
    return (Num() ^ CreateConstExpression) | 
          (Id() ^ CreateVariableExpression) |
          (Str() ^ CreateVariableStringExpression) |
          (Bool() ^ CreateBooleanStringExpression);
}

Combinator NotOperation() {
    auto MakeOperation = [](Result::Value result) {
        auto expresion = std::any_cast<std::shared_ptr<class Expression>>(result[1]);
        return Result::MakeValue(std::make_shared<UnaryOperationExpression>("not", expresion));
    };
    return KeyWord("not") + GetLazy(Expression) ^ MakeOperation;
}

Combinator CastOperation() {
    auto MakeCastOperation = [](Result::Value result) {
        std::string op = std::any_cast<std::string>(result[0]);
        auto expresion = std::any_cast<std::shared_ptr<class Expression>>(result[2]);
        return Result::MakeValue(std::make_shared<UnaryOperationExpression>(op, expresion));
    };
    return (KeyWord("cast_to_int") | KeyWord("cast_to_bool") | KeyWord("cast_to_str") | KeyWord("get_type")) +
            KeyWord("(") + GetLazy(Expression) + KeyWord(")") ^ MakeCastOperation;
}

Combinator UnaryOperation() {
    return NotOperation() | CastOperation();
}

Combinator ExpressionToWriter() {
    auto MakeWriter = [](Result::Value value) {
        auto expresion = std::any_cast<std::shared_ptr<class Expression>>(value[0]);
        return Result::MakeValue(std::make_shared<class WriteStatement>(expresion));
    };
    return std::make_shared<Processor>(Expression(), MakeWriter);
}

Combinator AddWriteSeparator() {
    auto Unitor = [](Result::Value left, Result::Value right) {
        auto left_expresion = std::any_cast<std::shared_ptr<Statement>>(left[0]);
        auto right_expresion = std::any_cast<std::shared_ptr<Statement>>(right[1]);
        return Result::MakeValue(std::make_shared<CompoundStatement>(left_expresion, right_expresion));
    };
    return std::make_shared<Separator>(ExpressionToWriter(), KeyWord(","), Unitor);
}

Combinator WriteStatement() {
    auto GetWriteStatement = [](const Result::Value& value) {
        return Result::Value{value[2]};
    };
    return KeyWord("write") + KeyWord("(") + AddWriteSeparator() + KeyWord(")") + KeyWord(";") ^ GetWriteStatement;
}

Combinator ExpressionToReader() {
    auto MakeReader = [](Result::Value value) {
        auto name = std::any_cast<std::string>(value[0]);
        return Result::MakeValue(std::make_shared<class ReadStatement>(name));
    };
    return Id() ^ MakeReader;
}

Combinator AddReadSeparator() {
    auto Unitor = [](Result::Value left, Result::Value right) {
        auto left_expresion = std::any_cast<std::shared_ptr<Statement>>(left[0]);
        auto right_expresion = std::any_cast<std::shared_ptr<Statement>>(right[1]);
        return Result::MakeValue(std::make_shared<CompoundStatement>(left_expresion, right_expresion));
    };
    return std::make_shared<Separator>(ExpressionToReader(), KeyWord(","), Unitor);
}

Combinator ReadStatement() {
    auto GetWriteStatement = [](const Result::Value& value) {
        return Result::Value{value[2]};
    };
    return KeyWord("read") + KeyWord("(") + AddReadSeparator() + KeyWord(")") + KeyWord(";") ^ GetWriteStatement;
}

Combinator ElseStatement() {
    auto MakeElseStatement = [](Result::Value value) {
        return Result::Value{value[2]};
    };
    return std::make_shared<Optional>(KeyWord("else") + KeyWord("begin") + GetLazy(Statements) + KeyWord("end") ^ MakeElseStatement);
}

Combinator IfStatement() {
    auto MakeIfStatement = [](Result::Value value) {
        auto condition = std::any_cast<std::shared_ptr<class Expression>>(value[1]);
        auto if_expresion = std::any_cast<std::shared_ptr<Statement>>(value[4]);
        std::shared_ptr<Statement> else_expresion;
        try {
            else_expresion = std::any_cast<std::shared_ptr<Statement>>(value[6]);
        } catch(...) {}
        return Result::MakeValue(std::make_shared<class IfStatement>(condition, if_expresion, else_expresion));
    };
    return KeyWord("if") + 
            GetLazy(Expression) + 
            KeyWord("then") + 
            KeyWord("begin") + 
            GetLazy(Statements) +
            KeyWord("end") +
            ElseStatement() +
            KeyWord(";") ^ MakeIfStatement;
}


Combinator WhileStatement() {
    auto MakeWhileStatement = [](Result::Value value) {
        auto condition = std::any_cast<std::shared_ptr<class Expression>>(value[1]);
        auto expresion = std::any_cast<std::shared_ptr<Statement>>(value[4]);
        return Result::MakeValue(std::make_shared<class WhileStatement>(condition, expresion));
    };
    return (KeyWord("while")) +
            GetLazy(Expression) +
            KeyWord("do") +
            KeyWord("begin") + 
            GetLazy(Statements) +
            KeyWord("end") + 
            KeyWord(";") ^ MakeWhileStatement;
}

Combinator ForStatement() {
    auto MakeForStatement = [](Result::Value value) {
        std::string variable = std::any_cast<std::string>(value[1]);
        std::shared_ptr<class Expression> start_expresion;
        start_expresion = std::any_cast<std::shared_ptr<class Expression>>(value[2]);
        bool is_to = std::any_cast<std::string>(value[3]) == "to";
        std::shared_ptr<class Expression> final_expresion = std::any_cast<std::shared_ptr<class Expression>>(value[4]);
        std::shared_ptr<Statement> statements = std::any_cast<std::shared_ptr<Statement>>(value[7]);

        return Result::MakeValue(std::make_shared<class ForStatement>(variable, start_expresion, final_expresion, statements, is_to));
    };
    auto GetExpresion = [](Result::Value value) {
        if (value.size() == 2) {
            return Result::Value{value[1]};
        }
        return Result::MakeValue(std::shared_ptr<class Expression>());
    };
    return KeyWord("for") +
            (Id()) +
            (GetOptional(KeyWord(":=") + GetLazy(Expression)) ^ GetExpresion) +
            (KeyWord("to") | KeyWord("downto")) +
            GetLazy(Expression) +
            KeyWord("do") +
            KeyWord("begin") + 
            GetLazy(Statements) +
            KeyWord("end") + 
            KeyWord(";") ^ MakeForStatement;
}


}