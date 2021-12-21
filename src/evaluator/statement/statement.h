#pragma once 

#include <memory>

#include <evaluator/statement/base_statement.h>
#include <evaluator/expression/base_expression.h>


class AssignStatement: public Statement {
public:
    AssignStatement(const std::string& name, 
                    const std::shared_ptr<Expression>& arithmetic_expression);
    void eval (std::unordered_map<std::string, DefaultType>&) override;

private:
    std::string name_;
    std::shared_ptr<Expression> arithmetic_expression_;
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(const std::shared_ptr<Statement>& left, 
                      const std::shared_ptr<Statement>& right);
    void eval(std::unordered_map<std::string, DefaultType>&) override;   

private:
    std::shared_ptr<Statement> left_;
    std::shared_ptr<Statement> right_;
};

class WriteStatement: public Statement {
public:

    WriteStatement(const std::shared_ptr<Expression>& expresion,
                   bool has_endln = false);
    void eval(std::unordered_map<std::string, DefaultType>&) override;

private:
    std::shared_ptr<Expression> expresion_;
};

class ReadStatement: public Statement {
public:
    ReadStatement(const std::string& name);
    void eval(std::unordered_map<std::string, DefaultType>&) override;
private:
    std::string name_;
};

class IfStatement: public Statement {
public:
    IfStatement(const std::shared_ptr<Expression>& condition,
                const std::shared_ptr<Statement>& if_statements,
                const std::shared_ptr<Statement>& else_statements);

    void eval(std::unordered_map<std::string, DefaultType>&) override;              

private:
    std::shared_ptr<Expression> condition_;
    std::shared_ptr<Statement> if_statements_;
    std::shared_ptr<Statement> else_statements_;
};

class WhileStatement: public Statement {
public:
    WhileStatement(const std::shared_ptr<Expression>& expresion,
                    const std::shared_ptr<Statement>& statements);
    void eval(std::unordered_map<std::string, DefaultType>&) override;

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
                    bool is_to);
    void eval(std::unordered_map<std::string, DefaultType>&) override;

private:
    std::string variable_name_;
    std::shared_ptr<Expression> start_expresion_;
    std::shared_ptr<Expression> final_expresion_;
    std::shared_ptr<Statement> statements_;
    int delta_ = 0;
};