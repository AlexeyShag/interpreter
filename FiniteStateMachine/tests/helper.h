#pragma once

#include <string>
#include <random>

std::string GetRandomString(size_t size,
                            const std::vector<char> alphabet) {
    
    static std::random_device rd;
    std::string s = "";
    for (size_t i = 0; i < size; ++i) {
        s.push_back(alphabet[rd() % alphabet.size()]);
    }
    return s;
}