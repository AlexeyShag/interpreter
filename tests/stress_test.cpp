#include <gtest/gtest.h>

#include <iostream>
#include <fstream>

#include <lexer/lexer.h>
#include <languge/parser.h>
#include <languge/init.h>

TEST(Test, stress_test) {

    std::ifstream fin("./../tests/test.imp");

    std::string code = "";
    
    for (std::string line; std::getline(fin, line);) {
        if (code.size()) {
            code += '\n';
        }
        code += line;
    }

    Lexer lexer = languge::GetLexer();
    auto tokens = lexer.SplitText(code);
    auto evaluator = languge::Parse(tokens);
    std::unordered_map<std::string, std::variant<int, std::string, bool>> env;
    evaluator->eval(env);
    if (auto ptr = std::get_if<int>(&env["i"])) {
        //EXPECT_EQ(*ptr, 0);
    } else {
      //  FAIL();
    }

    if (auto ptr = std::get_if<bool>(&env["a"])) {
      //  EXPECT_EQ(*ptr, true);
    } else {
       // FAIL();
    }


    if (auto ptr = std::get_if<int>(&env["b"])) {
       // EXPECT_EQ(*ptr, 0);
    } else {
       // FAIL();
    }

    if (auto ptr = std::get_if<std::string>(&env["c"])) {
      //  EXPECT_EQ(*ptr, "abc");
    } else {
       // FAIL();
    }
}