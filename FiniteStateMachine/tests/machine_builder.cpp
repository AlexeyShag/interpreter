#include <gtest/gtest.h>
#include <finite_state_machine/finite_state_machine.h>
#include <builder/machine_builder.h>

TEST(MachineBuilder, test_invalid_regular_expression) {
    MachineBuilder builder;

    std::string regular_expression = "ab*";
    ASSERT_THROW(builder.BuildFromRegularExpresion(regular_expression), std::runtime_error);
    regular_expression = "ab..";
    ASSERT_THROW(builder.BuildFromRegularExpresion(regular_expression), std::runtime_error);
    regular_expression = "*..adsadaaasd";
    ASSERT_THROW(builder.BuildFromRegularExpresion(regular_expression), std::runtime_error);
    regular_expression = "aa++";
    ASSERT_THROW(builder.BuildFromRegularExpresion(regular_expression), std::runtime_error);
    regular_expression = "aa2++";
    ASSERT_THROW(builder.BuildFromRegularExpresion(regular_expression), std::runtime_error);
    regular_expression = "ab+*";
    ASSERT_NO_THROW(builder.BuildFromRegularExpresion(regular_expression));
}

TEST(MachineBuilder, pretest) {
    MachineBuilder builder;
    
    std::string regular_expression = "ab+c.ab1.*.ba0.+.+*";
    auto machine = builder.BuildFromRegularExpresion(regular_expression);

    ASSERT_TRUE(1 == machine.GetNumberTerminateState());
    ASSERT_TRUE(7 == machine.GetNumberState());

    ASSERT_TRUE(machine.CheckingWord("ac"));
    ASSERT_TRUE(machine.CheckingWord("bc"));
    ASSERT_FALSE(machine.CheckingWord("cc"));
}