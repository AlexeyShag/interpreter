#pragma once

#include <unordered_map>
#include <memory>
#include <variant>


class Statement {
public:
    virtual void eval(std::unordered_map<std::string, DefaultType>&) = 0;
    virtual ~Statement() {}
};

class Expression {
public:
    virtual DefaultType eval(std::unordered_map<std::string, DefaultType>&) = 0;
    virtual ~Expression() {}
};

class ConstExpression: public Expression {
public:
    ConstExpression(int val) : val_(val) {}
    ConstExpression(const std::string& val) : val_(val) {}
    ConstExpression(bool val) : val_(val) {}

    DefaultType eval(std::unordered_map<std::string, DefaultType>&) {
        return val_;
    }
private:
    DefaultType val_;

};

class VariableExpression: public Expression {
public:
    VariableExpression(const std::string& name) : name_(name) {}
    DefaultType eval(std::unordered_map<std::string, DefaultType>& env) {
        return env[name_];
    }
private:
    std::string name_;

};

class UnaryOperationExpression: public Expression {
public:
    UnaryOperationExpression(std::string op, 
                            std::shared_ptr<Expression> expresion) 
                            : op_(op)
                            , expresion_(expresion) {}
    
    DefaultType eval(std::unordered_map<std::string, DefaultType>& env) {
        auto result = expresion_->eval(env);
        if (op_ == "not") {
            if (auto ans = std::get_if<bool>(&result)) {
                return !(*ans);
            } else {
                throw std::runtime_error("invalid operation");
            }
        } else if (op_ == "cast_to_bool") {
            if (auto ans = std::get_if<std::string>(&result)) {
                if (*ans == "true") {
                    return true;
                } else if (*ans == "false") {
                    return false;
                } else {
                    throw std::runtime_error("can't cast string to bool");
                }
            }
            if (auto ans = std::get_if<int>(&result)) {
                if (*ans != 0) {
                    return true;
                } else {
                    return false;
                }
            }
            return result;
        } else if (op_ == "cast_to_str") {
            if (auto ans = std::get_if<int>(&result)) {
                std::string str = "";
                while (*ans) {
                    str += *ans % 10 + '0';
                    *ans /= 10;
                }
                std::reverse(str.begin(), str.end());
                return str;
            }
            if (auto ans = std::get_if<bool>(&result)) {
                if (!*ans) {
                    return "true";
                } else {
                    return "false";
                }
            }
            return result;
        } else if (op_ == "cast_to_int") {
            if (auto ans = std::get_if<bool>(&result)) {
                if (*ans) {
                    return 1;
                } else {
                    return 0;
                }
            }
            if (auto ans = std::get_if<std::string>(&result)) {
                int i = 0;
                bool sign = false;
                int num = 0;
                if ((*ans)[i] == '-') {
                    i += 1;
                    sign = true;
                }
                while (i < (*ans).size()) {
                    if ((*ans)[i] >= '0' && (*ans)[i] <= '9') {
                        num *= 10;
                        num += (*ans)[i] - '0';
                    } else {
                        throw std::runtime_error("can't cast str to int");
                    }
                    
                    i += 1;
                }
                if (sign) {
                    num = -num;
                }
                return num;
            }
            return result;
        } else if (op_ == "get_type") {
            if (auto ans = std::get_if<std::string>(&result)) {
                return "str";
            } 
            if (auto ans = std::get_if<int>(&result)) {
                return "int";
            } 
            if (auto ans = std::get_if<bool>(&result)) {
                return "bool";
            } 
            throw std::runtime_error("unknown operator");
        } else {
            throw std::runtime_error("unknown operator");
        }
    }

private:
    std::string op_;
    std::shared_ptr<Expression> expresion_;
};

class BinaryOperationExpression: public Expression {
public:
    BinaryOperationExpression(std::string op, 
                                        std::shared_ptr<Expression> left,
                                        std::shared_ptr<Expression> right) 
                                            : op_(op)
                                            , left_(left)
                                            , right_(right) {}
    
    DefaultType eval(std::unordered_map<std::string, DefaultType>& env) {
        auto left_value = left_->eval(env);
        auto right_value = right_->eval(env);
        if (left_value.index() != right_value.index()) {
            throw std::runtime_error("can't do operathion with diffrent types");
        }
        if (auto left_v = std::get_if<int>(&left_value)) {
            auto right_v = std::get_if<int>(&right_value);
            DefaultType value = 0;
            if (op_ == "+") {
                value = *left_v + *right_v;
            } else if (op_ == "-") {
                value = *left_v - *right_v;
            } else if (op_ == "*") {
                value = *left_v * *right_v;
            } else if (op_ == "/") {
                value = *left_v / *right_v;
            } else if (op_ == "<") {
                value = *left_v < *right_v;
            } else if (op_ == "<=") {
                value = *left_v < *right_v;
            } else if (op_ == "<>") {
                value = *left_v != *right_v;
            } else if (op_ == "=") {
                value = *left_v == *right_v;
            } else if (op_ == ">") {
                value = *left_v > *right_v;
            } else if (op_ == ">=") {
                value = *left_v >= *right_v;
            } else {
                throw std::runtime_error("invalid operation");
            }
            return value;
        } 
        if (auto left_v = std::get_if<std::string>(&left_value)) {
            auto right_v = std::get_if<std::string>(&right_value);
            DefaultType value;
            if (op_ == "+") {
                value = *left_v + *right_v;
            } else if (op_ == "<") {
                value = *left_v < *right_v;
            } else if (op_ == "<=") {
                value = *left_v <= *right_v;
            } else if (op_ == "=") {
                value = *left_v == *right_v;
            } else if (op_ == "<") {
                value = *left_v < *right_v;
            } else if (op_ == "<>") {
                value = *left_v != *right_v;
            } else if (op_ == ">") {
                value = *left_v > *right_v;
            } else if (op_ == ">=") {
                value = *left_v >= *right_v;
            } else {
                throw std::runtime_error("invalid operation");
            }
            return value;
        } 
        if (auto left_v = std::get_if<bool>(&left_value)) {
            auto right_v = std::get_if<bool>(&right_value);
            bool value;
            if (op_ == "and") {
                value = *left_v & *right_v;
            } else if (op_ == "or") {
                value = *left_v | *right_v;
            } else if (op_ == "xor") {
                value = *left_v ^ *right_v;
            } else {
                throw std::runtime_error("invalid operation");
            }
            return value;
        } 
        return DefaultType();
    }

private:
    std::string op_;
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

class AssignStatement: public Statement {
public:
    AssignStatement(const std::string& name, 
                    std::shared_ptr<Expression> arithmetic_expression)
                        : name_(name)
                        , arithmetic_expression_ (arithmetic_expression) {}
    void eval (std::unordered_map<std::string, DefaultType>& env) {
        auto val = arithmetic_expression_->eval(env);
        env[name_] = val;
    }
private:
    std::string name_;
    std::shared_ptr<Expression> arithmetic_expression_;
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(std::shared_ptr<Statement> left, 
                        std::shared_ptr<Statement> right) : left_(left)
                                                            , right_(right) {}

    void eval(std::unordered_map<std::string, DefaultType>& env) {
        left_->eval(env);
        right_->eval(env);
    }
                                            
private:
    std::shared_ptr<Statement> left_;
    std::shared_ptr<Statement> right_;
};

class WriteStatement: public Statement {
public:
WriteStatement() {}
    WriteStatement(std::shared_ptr<Expression> expresion,
                    bool has_endln = false)
                    : expresion_(expresion) {}

    void eval(std::unordered_map<std::string, DefaultType>& env) {
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
private:
    std::shared_ptr<Expression> expresion_;
};


class ReadStatement: public Statement {
public:
    ReadStatement() {}
    ReadStatement(const std::string& name)
                    : name_(name) {}

    void eval(std::unordered_map<std::string, DefaultType>& env) {
        std::string s;
        std::cin >> s;
        env[name_] = s;
    }
private:
    std::string name_;
};

class IfStatement: public Statement {
public:
    IfStatement(const std::shared_ptr<Expression>& condition,
                const std::shared_ptr<Statement>& if_statements,
                const std::shared_ptr<Statement>& else_statements)
                    : condition_(condition)
                    , if_statements_(if_statements)
                    , else_statements_(else_statements) {}

    void eval(std::unordered_map<std::string, DefaultType>& env) {
       DefaultType result = condition_->eval(env);
       if (auto ans = std::get_if<bool>(&result)) {
           std::cerr << *ans << '\n';
           if (*ans) {
               std::cerr << "if" << '\n';
               if_statements_->eval(env);
           } else if (else_statements_) {
               std::cerr << "else" << '\n';
               else_statements_->eval(env);
           }
       } else {
           throw std::runtime_error("expected condition that return boolean");
       }
    }                

private:
    std::shared_ptr<Expression> condition_;
    std::shared_ptr<Statement> if_statements_;
    std::shared_ptr<Statement> else_statements_;
};

class WhileStatement: public Statement {
public:
    WhileStatement(const std::shared_ptr<Expression>& expresion,
                    const std::shared_ptr<Statement>& statements)
                            : expresion_(expresion)
                            , statements_(statements) {}

    void eval(std::unordered_map<std::string, DefaultType>& env) {
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
private:
    std::shared_ptr<Expression> expresion_;
    std::shared_ptr<Statement> statements_;
};

class ForStatement: public Statement {
public:
    ForStatement(const std::string& variable_name,
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

    void eval(std::unordered_map<std::string, DefaultType>& env) {
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
private:
    std::string variable_name_;
    std::shared_ptr<Expression> start_expresion_;
    std::shared_ptr<Expression> final_expresion_;
    std::shared_ptr<Statement> statements_;
    int delta_ = 0;
};