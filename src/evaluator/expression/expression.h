#pragma once 

#include <unordered_map>
#include <memory>

#include <evaluator/expression/base_expression.h>

class ConstExpression: public Expression {
public:
    ConstExpression(int val);
    ConstExpression(const std::string& val);
    ConstExpression(bool val);
    DefaultType eval(std::unordered_map<std::string, DefaultType>&) override;

private:
    DefaultType val_;
};

class VariableExpression: public Expression {
public:
    VariableExpression(const std::string& name);
    DefaultType eval(std::unordered_map<std::string, DefaultType>& env) override;

private:
    std::string name_;
};

class UnaryOperationExpression: public Expression {
public:
    UnaryOperationExpression(std::string op, 
                             std::shared_ptr<Expression> expresion);
    DefaultType eval(std::unordered_map<std::string, DefaultType>& env) override;

private:
    std::string op_;
    std::shared_ptr<Expression> expresion_;
};

class BinaryOperationExpression: public Expression {
public:
    BinaryOperationExpression(std::string op, 
                              std::shared_ptr<Expression> left,
                              std::shared_ptr<Expression> right);
    DefaultType eval(std::unordered_map<std::string, DefaultType>& env) override;

private:
    std::string op_;
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};