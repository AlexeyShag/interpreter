#include <iostream>

#include <evaluator/statement/statement.h>


AssignStatement::AssignStatement(const std::string& name, 
                                 const std::shared_ptr<Expression>& arithmetic_expression)
                                    : name_(name)
                                    , arithmetic_expression_ (arithmetic_expression) {}

void AssignStatement::eval(std::unordered_map<std::string, DefaultType>& env) {
    auto val = arithmetic_expression_->eval(env);
    env[name_] = val;
}

CompoundStatement::CompoundStatement(const std::shared_ptr<Statement>& left, 
                                     const std::shared_ptr<Statement>& right) 
                                        : left_(left)
                                        , right_(right) {}

void CompoundStatement::eval(std::unordered_map<std::string, DefaultType>& env) {
    left_->eval(env);
    right_->eval(env);
}
         
WriteStatement::WriteStatement(const std::shared_ptr<Expression>& expresion,
                               bool has_endln)
                                    : expresion_(expresion) {}

void WriteStatement::eval(std::unordered_map<std::string, DefaultType>& env) {
    DefaultType val = expresion_->eval(env);
    if (auto ptr = std::get_if<int>(&val)) {
        std::cout << *ptr;
    }
    if (auto ptr = std::get_if<std::string>(&val)) {
        size_t new_size = 0; 
        for (size_t i = 0; i < (*ptr).size(); ++i) {
            if (i && (*ptr)[i-1] == '\\') {
                if ((*ptr)[i] == 'n') {
                    (*ptr)[new_size - 1] = '\n';
                } else if ((*ptr)[i] == 't') {
                    (*ptr)[new_size - 1] = '\t';
                } else {
                    (*ptr)[new_size - 1] = (*ptr)[i];
                }
                ++i;
                continue;
            } else {
                (*ptr)[new_size++] = (*ptr)[i];
            }
        }
        ptr->resize(new_size);
        std::cout << *ptr;
    }
    if (auto ptr = std::get_if<bool>(&val)) {
        std::cout << ( *ptr ? "true" : "false");
    }
}
        
ReadStatement::ReadStatement(const std::string& name)
                                : name_(name) {}

void ReadStatement::eval(std::unordered_map<std::string, DefaultType>& env) {
    std::string s;
    std::cin >> s;
    env[name_] = s;
}

IfStatement::IfStatement(const std::shared_ptr<Expression>& condition,
                         const std::shared_ptr<Statement>& if_statements,
                         const std::shared_ptr<Statement>& else_statements)
                                : condition_(condition)
                                , if_statements_(if_statements)
                                , else_statements_(else_statements) {}

void IfStatement::eval(std::unordered_map<std::string, DefaultType>& env) {
    DefaultType result = condition_->eval(env);
    if (auto ans = std::get_if<bool>(&result)) {
        if (*ans) {
            if_statements_->eval(env);
        } else if (else_statements_) {
            else_statements_->eval(env);
        }
    } else {
        throw std::runtime_error("expected condition that return boolean");
    }
}  

WhileStatement::WhileStatement(const std::shared_ptr<Expression>& expresion,
                               const std::shared_ptr<Statement>& statements)
                                    : expresion_(expresion)
                                    , statements_(statements) {}

void WhileStatement::eval(std::unordered_map<std::string, DefaultType>& env) {
    while (1) {
        auto ans = expresion_->eval(env);
        auto result = std::get_if<bool>(&(ans));
        if (!result) {
            throw std::runtime_error("while expected boolean expresion");
        }
        if (!*result) {
            break;
        }
        statements_->eval(env);
    }
}

ForStatement::ForStatement(const std::string& variable_name,
                           const std::shared_ptr<Expression>& start_expresion,
                           const std::shared_ptr<Expression>& final_expresion,
                           const std::shared_ptr<Statement>& statements,
                           bool is_to)
                                : variable_name_(variable_name)
                                , start_expresion_(start_expresion)
                                , final_expresion_(final_expresion)
                                , statements_(statements) {
                                    delta_ = is_to ? 1 : -1;
                                }

void ForStatement::eval(std::unordered_map<std::string, DefaultType>& env) {
    auto* variable = std::get_if<int>(&env[variable_name_]);
    if (!variable) {
        throw std::runtime_error("operator for expected int expresion");
    }
    if (start_expresion_) {
        auto start_exptresion_result = start_expresion_->eval(env);
        int* expresion = std::get_if<int>(&start_exptresion_result);
        if (!expresion) {
            throw std::runtime_error("operator for expected int expresion");
        }
        *variable = *expresion;
    }
    DefaultType result = final_expresion_->eval(env);
    int* res = std::get_if<int>(&result);
    if (!res) {
        throw std::runtime_error("operator for expected int expresion");
    }
    while (*variable != *res) {
        statements_->eval(env);
        *variable += delta_;
    }
    statements_->eval(env);
}