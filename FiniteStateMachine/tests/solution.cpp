#include <gtest/gtest.h>
#include <solution/solution.h>

TEST(Solution, pretest) {
    Solution solution;

    ASSERT_TRUE(solution.Solve("ab+c.aba.*.bac.+.+*", "babc") == 3);
    ASSERT_TRUE(solution.Solve("acb..bab.c.*.ab.ba.+.+*a.", "abbaa") == 5);
}