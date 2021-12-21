#include <gtest/gtest.h>
#include <builder/suffix_machine_builder.h>
#include <tests/helper.h>

TEST(SuffixMachineBuilder, stresstest) {
    SuffixMachineBuilder builder;
    
    for (int i = 0; i < 100; ++i) {
        std::string s = GetRandomString(10, {'a', 'b', 'c'});
        auto machine = builder.Build(s);
        std::reverse(s.begin(), s.end());
        std::string suffics;

        for (const char symbol: s) {
            suffics = symbol + suffics;
            ASSERT_TRUE(machine.CheckingWord(suffics));
        }
    }
}
