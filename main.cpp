#include <iostream>
#include <fstream>

#include <lexer/lexer.h>
#include <languge/parser.h>
#include <languge/init.h>

signed main(int argument_number, char* argument[]) {

    std::ifstream fin(argument[1]);

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
    
    return 0;
}