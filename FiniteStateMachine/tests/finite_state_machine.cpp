#include <gtest/gtest.h>
#include <finite_state_machine/finite_state_machine.h>

TEST(FiniteStateMachine, test_deleting_empty_transitions) {
	FiniteStateMachine machine;
	machine.SetNumberState(2)
			.AddAlpabet(std::vector<char>({'a'}))
			.SetInitialState(0)
			.SetTerminateStates(std::vector<size_t>({1}))
			.AddTransition(0, 'a', 1)
			.AddTransition(0, eps, 1);

	machine.DeleteEmptyTransitions();
	EXPECT_EQ(2, machine.GetNumberState());
	EXPECT_EQ(1, machine.GetNumberTransitions());
}

TEST(FiniteStateMachine, test_determinictic) {
	FiniteStateMachine machine;
	machine.SetNumberState(4)
			.AddAlpabet(std::vector<char>({'a', 'b'}))
			.SetInitialState(0)
			.SetTerminateStates(std::vector<size_t>({3}))
			.AddTransition(0, 'a', 0)
			.AddTransition(0, 'b', 0)
			.AddTransition(0, 'b', 1)
			.AddTransition(1, 'a', 2)
			.AddTransition(1, 'b', 2)
			.AddTransition(2, 'a', 3)
			.AddTransition(2, 'b', 3);

	machine.MakeDeterministic();

	EXPECT_EQ(8, machine.GetNumberState());
	EXPECT_EQ(16, machine.GetNumberTransitions());
	EXPECT_EQ(4, machine.GetNumberTerminateState());

	machine.Clear();
    machine.SetNumberState(8)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({2,3,6}))
        .AddTransition(0, 'b', 1)
        .AddTransition(0, 'a', 3)
        .AddTransition(1, 'a', 2)
        .AddTransition(3, 'a', 4)
        .AddTransition(4, 'a', 5)
        .AddTransition(5, 'b', 4)
        .AddTransition(4, 'a', 6)
        .AddTransition(6, 'a', 4)
        .AddTransition(6, 'a', 7)
        .AddTransition(7, 'b', 6);
    machine.MakeDeterministic();

    EXPECT_EQ(8, machine.GetNumberState());
    EXPECT_EQ(10, machine.GetNumberTransitions());
    EXPECT_EQ(4, machine.GetNumberTerminateState());
}

TEST(FiniteStateMachine, test_full_deterministic) {
    FiniteStateMachine machine;
    machine.SetNumberState(3)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({0,1}))
        .AddTransition(0, 'a', 1)
        .AddTransition(1, 'b', 0)
        .AddTransition(0, 'b', 2)
        .AddTransition(2, 'a', 0);

    machine.MakeFullDeterministic();

    EXPECT_EQ(4, machine.GetNumberState());
    EXPECT_EQ(8, machine.GetNumberTransitions());
}

TEST(FiniteStateMachine, test_addition) {
    FiniteStateMachine machine;
    machine.SetNumberState(4)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({0,1}))
        .AddTransition(0, 'a', 1)
        .AddTransition(1, 'b', 0)
        .AddTransition(1, 'a', 2)       
        .AddTransition(0, 'b', 2)
        .AddTransition(2, 'a', 0)
        .AddTransition(2, 'b', 3)
        .AddTransition(3, 'b', 3)
        .AddTransition(3, 'a', 3);

    machine.MakeAddition();

    EXPECT_EQ(4, machine.GetNumberState());
    EXPECT_EQ(8, machine.GetNumberTransitions());
    EXPECT_EQ(2, machine.GetNumberTerminateState());
}

TEST(FiniteStateMachine, test_min_full_deterministic) {
    FiniteStateMachine machine;
    machine.SetNumberState(9)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({0,4,8}))
        .AddTransition(0, 'a', 1)
        .AddTransition(1, 'a', 2)
        .AddTransition(3, 'a', 4)
        .AddTransition(4, 'a', 5)
        .AddTransition(6, 'a', 7)
        .AddTransition(7, 'a', 8)
        .AddTransition(0, 'b', 3)
        .AddTransition(1, 'b', 4)
        .AddTransition(2, 'b', 5)
        .AddTransition(3, 'b', 6)
        .AddTransition(4, 'b', 7)
        .AddTransition(5, 'b', 8)
        .AddTransition(2, 'a', 0)
        .AddTransition(5, 'a', 3)
        .AddTransition(8, 'a', 6)
        .AddTransition(8, 'b', 2)
        .AddTransition(7, 'b', 1)
        .AddTransition(6, 'b', 0);

    machine.MakeMinFullDeterministic();

    EXPECT_EQ(3, machine.GetNumberState());
    EXPECT_EQ(6, machine.GetNumberTransitions());
    EXPECT_EQ(1, machine.GetNumberTerminateState());
}

TEST(FiniteStateMachine, test_add_exists_state) {
    FiniteStateMachine machine;
    ASSERT_THROW(machine.SetNumberState(2)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({1}))
        .AddTransition(1, 'a', 0)
        .AddTransition(0, 'b', 1)
        .AddNewState(1), std::runtime_error);
}

TEST(FiniteStateMachine, test_cheking_word) {
    FiniteStateMachine machine;
    machine.SetNumberState(4)
            .AddAlpabet(std::vector<char>({'a', 'b'}))
            .SetInitialState(0)
            .SetTerminateStates(std::vector<size_t>({3}))
            .AddTransition(0, 'a', 1)
            .AddTransition(1, eps, 2)
            .AddTransition(2, 'b', 3);


    ASSERT_TRUE(machine.CheckingWord("ab"));
    ASSERT_FALSE(machine.CheckingWord("aa"));
    ASSERT_FALSE(machine.CheckingWord("b"));
    ASSERT_FALSE(machine.CheckingWord("a"));
}

TEST(FiniteStateMachine, test_catcatination_of_machine) {
    FiniteStateMachine machine1;
    machine1.SetNumberState(4)
            .AddAlpabet(std::vector<char>({'a', 'b'}))
            .SetInitialState(0)
            .SetTerminateStates(std::vector<size_t>({3}))
            .AddTransition(0, 'b', 1)
            .AddTransition(0, 'a', 2)
            .AddTransition(1, 'a', 3)
            .AddTransition(2, 'b', 3);

    FiniteStateMachine machine2;
    machine2.SetNumberState(3)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({1, 2}))
        .AddTransition(0, 'b', 1)
        .AddTransition(0, 'a', 2);
    
    machine1.Cat(machine2);

    std::unordered_set<std::string> set = {
        "aba", "abb", "baa", "bab"
    };
    auto parse = [](size_t mask) {
        std::string s;
        for (size_t i = 0;i < 3; ++i) {
            s += 'a' + (mask & (1 << i));
        }
        return s;
    };
    for (size_t mask = 0; mask < 8; ++mask) {
        auto s = parse(mask);
        ASSERT_TRUE(machine1.CheckingWord(s) == (set.count(s) != 0));
    }
    
}

TEST(FiniteStateMachine, test_sum_of_machine) {
    FiniteStateMachine machine1;
    machine1.SetNumberState(4)
            .AddAlpabet(std::vector<char>({'a', 'b'}))
            .SetInitialState(0)
            .SetTerminateStates(std::vector<size_t>({3}))
            .AddTransition(0, 'b', 1)
            .AddTransition(0, 'a', 2)
            .AddTransition(1, 'a', 3)
            .AddTransition(2, 'b', 3);

    FiniteStateMachine machine2;
    machine2.SetNumberState(2)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({1}))
        .AddTransition(1, 'a', 0);
    
    machine1.Sum(machine2);

    std::unordered_set<std::string> set = {
        "abaab", "ababa", "baaab", "baaba"
    };
    auto parse = [](size_t mask) {
        std::string s;
        for (size_t i = 0;i < 5; ++i) {
            s += 'a' + (mask & (1 << i));
        }
        return s;
    };
    for (size_t mask = 0; mask < 32; ++mask) {
        auto s = parse(mask);
        ASSERT_TRUE(machine1.CheckingWord(s) == (set.count(s) != 0));
    }
}

TEST(FiniteStateMachine, test_merge_invalid_machines) {
    FiniteStateMachine machine1;
    machine1.SetNumberState(3)
            .AddAlpabet(std::vector<char>({'a', 'b'}))
            .SetInitialState(0)
            .SetTerminateStates(std::vector<size_t>({1, 2}))
            .AddTransition(0, 'b', 1)
            .AddTransition(0, 'a', 2);

    FiniteStateMachine machine2;
    machine2.SetNumberState(3)
        .AddAlpabet(std::vector<char>({'a', 'b'}))
        .SetInitialState(0)
        .SetTerminateStates(std::vector<size_t>({1, 2}))
        .AddTransition(0, 'b', 1)
        .AddTransition(0, 'a', 2);


    ASSERT_THROW(machine1.Cat(machine2), std::runtime_error);
    ASSERT_THROW(machine1.Sum(machine2), std::runtime_error);
    ASSERT_THROW(machine1.MakeCircle(), std::runtime_error);

    machine2.Clear();
    machine1.Clear();

    machine2.SetNumberState(1)
            .AddAlpabet(std::vector<char>({'a', 'b'}))
            .SetInitialState(0)
            .SetTerminateStates(std::vector<size_t>({0}));
    ASSERT_THROW(machine1.Sum(machine2), std::runtime_error);


    machine1.SetNumberState(1)
            .AddAlpabet(std::vector<char>({'a', 'b'}))
            .SetInitialState(0)
            .SetTerminateStates(std::vector<size_t>({0}));
    ASSERT_THROW(machine1.Sum(machine2), std::runtime_error);
    machine1.AddNewState(1)
            .SetTerminateStates(std::vector<size_t>({1}));
    ASSERT_THROW(machine1.Sum(machine2), std::runtime_error);
}
