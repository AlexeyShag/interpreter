#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <map>
#include <iostream>
#include <string>

#define eps 0

class FiniteStateMachine {
public:
	FiniteStateMachine() {};

	FiniteStateMachine(const FiniteStateMachine&) = default;
	FiniteStateMachine(FiniteStateMachine&&) = default;
	FiniteStateMachine& operator=(const FiniteStateMachine&) = default;
	FiniteStateMachine& operator=(FiniteStateMachine&&) = default;

	FiniteStateMachine& SetNumberState(size_t n);

	template<typename Set = std::vector<char>>
	FiniteStateMachine& AddAlpabet(Set&& alphabet) {
		sigma_ = std::forward<Set>(alphabet);
		return *this;
	}

	template<typename Set = std::vector<size_t>>
	FiniteStateMachine& SetTerminateStates(Set&& t) {
		terminated_state_ = std::forward<Set>(t);
		return *this;
	}

	FiniteStateMachine& AddTerminated(size_t state);

	FiniteStateMachine& SetInitialState(size_t i);
	FiniteStateMachine& AddTransition(size_t p, char c, size_t q);

	size_t GetNumberTransitions() const;
	size_t GetNumberState() const;
	size_t GetNumberTerminateState() const;

	size_t GetInitialState() const;
	
	FiniteStateMachine& AddNewState(size_t new_state);
	FiniteStateMachine& AddNewState(size_t new_state, std::unordered_map<char, std::vector<size_t>> transition);

	void Clear();

	bool CheckingWord(const std::string& s);

	void DeleteEmptyTransitions();
	void MakeDeterministic();
	void MakeFullDeterministic();

	void MakeAddition();

	void MakeMinFullDeterministic();

	void print() const;

	void Cat(const FiniteStateMachine& machine);
	void Sum(const FiniteStateMachine& machine);
	void MakeCircle();
	
	const std::unordered_map<char, std::vector<size_t>>& operator[](size_t i) const;

	void AddString(const std::string& word) {
		size_t now_state = initial_state_;
		for (char symbol: word) {
			if (delta_[now_state].find(symbol) != delta_[now_state].end()) {
				now_state = delta_[now_state][symbol][0];
			} else {
				AddNewState(states_.size());
				delta_[now_state][symbol] = {states_.size() - 1};
				now_state = states_.size() - 1;
			}
		}
		AddTerminated(now_state);
	}

	template<typename StringLike>
	int MatchPrefix(const StringLike& word, size_t state = 0, int ans = 0) const {
		if (word.size() == ans) {
			return ans;
		}
		try {
			if (delta_[state].at(word[ans]).size() == 0) {
				return ans;
			}
			size_t next_state = delta_[state].at(word[ans])[0];
			int res =  MatchPrefix(word, next_state, ans + 1);
			if (res == ans + 1) {
				for (char f_state: terminated_state_) {
					if (next_state == f_state) {
						return res;
					}
				}
				return ans;
			} else {
				return res;
			}
		} catch(...) {
			return ans;
		}
	}

private:
	bool dfs(size_t current_state, const std::string& word, size_t id);

	void Expand();

	std::vector<std::vector<size_t>> BuildEpsClosure();

	size_t UniqueStates(const std::vector<size_t>& states);
	size_t ExpandAndUniqueStates(const std::vector<size_t>& states);
	
	void Zip();

	void MergeAlphabet(const std::vector<char>& alphabet);

	bool IsTerminate(size_t state) const;

	std::vector<size_t> states_; // states
	std::vector<char> sigma_; // alphabet
	std::vector<std::unordered_map<char, std::vector<size_t>>> delta_; // transitions
	size_t initial_state_; // initial states
	std::vector<size_t> terminated_state_; // terminated states
};



