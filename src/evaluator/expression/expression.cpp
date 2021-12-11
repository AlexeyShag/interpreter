#include <evaluator/expression/expression.h>


ConstExpression::ConstExpression(int val) 
                                : val_(val) {}
ConstExpression::ConstExpression(const std::string& val) 
                                : val_(val) {}
ConstExpression::ConstExpression(bool val) 
                                : val_(val) {}

DefaultType ConstExpression::eval(std::unordered_map<std::string, DefaultType>&) {
    return val_;
}


VariableExpression::VariableExpression(const std::string& name) 
                                : name_(name) {}
DefaultType VariableExpression::eval(std::unordered_map<std::string, DefaultType>& env) {
    return env[name_];
}


UnaryOperationExpression::UnaryOperationExpression(std::string op, 
                            std::shared_ptr<Expression> expresion) 
                                : op_(op)
                                , expresion_(expresion) {}
    
DefaultType UnaryOperationExpression::eval(std::unordered_map<std::string, DefaultType>& env) {
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


BinaryOperationExpression::BinaryOperationExpression(std::string op, 
                                                     std::shared_ptr<Expression> left,
                                                     std::shared_ptr<Expression> right) 
                                            : op_(op)
                                            , left_(left)
                                            , right_(right) {}
    
DefaultType BinaryOperationExpression::eval(std::unordered_map<std::string, DefaultType>& env) {
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
            value = *left_v <= *right_v;
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