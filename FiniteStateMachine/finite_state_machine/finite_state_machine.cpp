
#include <finite_state_machine/finite_state_machine.h>

std::string ParseSymbol(char c) {
	if ('a' <= c && 'z' >= c) {
		return std::string(1, c);
	} else {
		return "eps";
	}
}

FiniteStateMachine& FiniteStateMachine::SetNumberState(size_t n) {
    states_.resize(n);
    delta_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        states_[i] = i;
    }
    return *this;
}

FiniteStateMachine& FiniteStateMachine::SetInitialState(size_t i) {
    initial_state_ = i;
    return *this;
}

FiniteStateMachine& FiniteStateMachine::AddTransition(size_t p, char c, size_t q) {
    delta_[p][c].push_back(q);
    return *this;
}

size_t FiniteStateMachine::GetNumberTransitions() const {
    size_t ans = 0;
    for (auto& edges: delta_) {
        ans += edges.size();
    }
    return ans;
}

size_t FiniteStateMachine::GetNumberState() const {
    return states_.size();
}

size_t FiniteStateMachine::GetNumberTerminateState() const {
    return terminated_state_.size();
}
	
FiniteStateMachine& FiniteStateMachine::AddNewState(size_t new_state) {
    if (delta_.size() > new_state && delta_[new_state].size() != 0) {
        throw std::runtime_error("This state is already exists");
    }
    delta_.resize(std::max(delta_.size(), new_state + 1));
    states_.push_back(new_state);
    return *this;
}

FiniteStateMachine& FiniteStateMachine::AddNewState(size_t new_state, std::unordered_map<char, std::vector<size_t>> transition) {
    AddNewState(new_state);
    delta_[new_state] = std::move(transition);
    return *this;
}


void FiniteStateMachine::Clear() {
    initial_state_ = 0;
    delta_.clear();
    states_.clear();
    terminated_state_.clear();
    sigma_.clear();
}

FiniteStateMachine& FiniteStateMachine::AddTerminated(size_t state) {
    terminated_state_.push_back(state);
    return *this;
}

const std::unordered_map<char, std::vector<size_t>>& FiniteStateMachine::operator[](size_t i) const {
    return delta_[i];
}
	

bool FiniteStateMachine::CheckingWord(const std::string& s) {
    return dfs(initial_state_, s, 0);
}

size_t FiniteStateMachine::GetInitialState() const {
    return initial_state_;
}

void FiniteStateMachine::DeleteEmptyTransitions() {
    Expand();
    auto eps_closure = BuildEpsClosure();
    
    size_t new_i = UniqueStates(eps_closure[initial_state_]);
    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta (1 << states_.size());
    std::vector<size_t> new_q;
    size_t f = UniqueStates(terminated_state_);
    std::vector<size_t> new_f;

    std::queue<size_t> buffer;
    std::unordered_set<size_t> was_in;
    buffer.push(new_i);
    while (!buffer.empty()) {
        auto current_state = buffer.front();
        buffer.pop();
        if (was_in.find(current_state) != was_in.end()) {
            continue;
        } 
        was_in.insert(current_state);
        new_q.push_back(current_state);
        if (f & current_state) {
            new_f.push_back(current_state);
        }
        for (auto symbol: sigma_) {
            size_t new_state = 0;
            for (size_t state_id = 0; state_id < states_.size(); ++state_id) {
                size_t state = (1 << state_id);
                if (current_state & state) {
                    for (auto& next_old_state: delta_[state][symbol]) {
                        new_state |= UniqueStates(eps_closure[next_old_state]);	
                    }
                }
            }
            if (new_state) {
                buffer.push(new_state);
                new_delta[current_state][symbol].push_back(new_state);
            }
        }
    }
    initial_state_ = new_i;
    states_ = std::move(new_q);
    delta_ = std::move(new_delta);
    terminated_state_ = std::move(new_f);

    Zip();
}

void FiniteStateMachine::MakeDeterministic() {
    Expand();

    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta(1 << states_.size());
    std::vector<size_t> new_q;
    size_t f = UniqueStates(terminated_state_);
    std::vector<size_t> new_f;

    std::queue<size_t> buffer;
    std::unordered_set<size_t> was_in;
    buffer.push(initial_state_);
    while (!buffer.empty()) {
        auto current_state = buffer.front();
        buffer.pop();
        if (was_in.find(current_state) != was_in.end()) {
            continue;
        } 
        was_in.insert(current_state);
        new_q.push_back(current_state);
        if (f & current_state) {
            new_f.push_back(current_state);
        }
        for (auto symbol: sigma_) {
            size_t new_state = 0;
            for (size_t state_id = 0; state_id < states_.size(); ++state_id) {
                size_t state = (1 << state_id);
                if (current_state & state) {
                    new_state |= UniqueStates(delta_[state][symbol]);	
                }
            }
            if (new_state) {
                buffer.push(new_state);
                new_delta[current_state][symbol].push_back(new_state);
            }
        }
    }

    states_ = std::move(new_q);
    delta_ = std::move(new_delta);
    terminated_state_ = std::move(new_f);

    Zip();
}

void FiniteStateMachine::MakeFullDeterministic() {
    size_t new_state = states_.size();
    bool is_new_state_created = false;
    for (auto& state: states_) {
        for (auto symbol: sigma_) {
            if (delta_[state].find(symbol) == delta_[state].end()) {
                delta_[state][symbol].push_back(new_state);
                is_new_state_created = true;
            }
        }
    }
    if (is_new_state_created) {	
        delta_.resize(states_.size() + 1);
        for (auto symbol: sigma_) {
            delta_[new_state][symbol].push_back(new_state);
        }
        states_.push_back(new_state);
    }
}	

void FiniteStateMachine::MakeAddition() {
    size_t uniqued_t = ExpandAndUniqueStates(terminated_state_);
    terminated_state_.clear();
    for (size_t state_id = 0; state_id < states_.size(); ++state_id) {
        if (!(uniqued_t & (1 << state_id))) {
            terminated_state_.push_back(state_id);
        }
    }
}

void FiniteStateMachine::MakeMinFullDeterministic() {
    Expand();
    std::vector<size_t> classes((1 << states_.size()), 1);
    for (auto& state: terminated_state_) {
        classes[state] = 2;
    }
    size_t number_classes = 2;
    size_t last_number_classes = -1;
    while (number_classes != last_number_classes) {
        last_number_classes = number_classes;
        std::map<std::vector<size_t>, std::vector<size_t>> new_classes;
        for (auto& current_state: states_) {
            std::vector<size_t> new_class;
            for (char symbol: sigma_) {
                size_t symbol_class = 0;
                for (auto& next_state: delta_[current_state][symbol]) {
                    symbol_class |= classes[next_state];
                }
                new_class.push_back(symbol_class);
            }
            
            new_classes[new_class].push_back(current_state);
        }
        size_t id = 1;
        number_classes = 0;
        for (auto& [_, set]: new_classes) {
            for (auto& state: set) {
                classes[state] = id;
            }
            id <<= 1;
            ++number_classes;
        }
    }
    initial_state_ = classes[initial_state_];

    states_.resize(number_classes);
    for (size_t i = 0; i < states_.size(); ++i) {
        states_[i] = (1 << i);
    }

    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta(1 << number_classes);
    for (size_t state = 0; state < delta_.size(); ++state) {
        for (auto& [symbol, next_states]: delta_[state]) {
            for (auto& next_state: next_states) {
                new_delta[classes[state]][symbol].push_back(classes[next_state]);
            }
            auto& set = new_delta[classes[state]][symbol];
            std::sort(set.begin(), set.end());
            set.erase(std::unique(set.begin(), set.end()), set.end());
        }
    }

    delta_ = std::move(new_delta);

    for(auto& state: terminated_state_) {
        state = classes[state];
    }
    std::sort(terminated_state_.begin(), terminated_state_.end());
    terminated_state_.erase(std::unique(terminated_state_.begin(), terminated_state_.end()), terminated_state_.end());

    Zip();
}	

void FiniteStateMachine::print() const {
    std::cerr << "State:\n";
    for (auto& state: states_) {
        std::cerr << state << ' ';
    }
    std::cerr << '\n';
    std::cerr << "\nDelta:\n";
    for (size_t state = 0; state < delta_.size(); ++state) {
        for (auto& iter: delta_[state]) {
            auto& symbol = iter.first;
            auto& next_states = iter.second;
            for (auto& next_state: next_states) {
                std::cerr << state << '-' << ParseSymbol(symbol) << '-' << next_state << '\n';
            }
        }
    }
    std::cerr << "\nInitial:\n";
    std::cerr << initial_state_ << '\n';
    std::cerr << "\nTerminate:\n";
    for (auto& state: terminated_state_) {
        std::cerr << state << ' ';
    }
    std::cerr << '\n';

    std::cerr << "================\n";

}

void FiniteStateMachine::Cat(const FiniteStateMachine& machine) {
    if (terminated_state_.size() != 1) {
        throw std::runtime_error("for catcatination need machines with 1 tinish state");
    }

    MergeAlphabet(machine.sigma_);

    size_t shift = states_.size();
    delta_.resize(states_.size() + machine.states_.size());

    for (const auto& state : machine.states_) {
        if (state != machine.initial_state_) {
            states_.push_back(state + shift);	
        }
    }

    for (size_t state = 0; state < machine.delta_.size(); ++state) {
        for (const auto& [symbol, next_states]: machine.delta_[state]) {
            for (const auto& next_state: next_states) {
                size_t v = state + shift;
                size_t u = next_state + shift;
                if (state == machine.initial_state_) {
                    v = terminated_state_[0];
                }
                if (next_state == machine.initial_state_) {
                    u = terminated_state_[0];
                }
                delta_[v][symbol].push_back(u);
            }
        }
    }

    if (machine.initial_state_ != machine.terminated_state_[0]) {
        terminated_state_ = machine.terminated_state_;
        for (auto& state: terminated_state_) {
            state += shift;
        }
    }

    Zip();
}

void FiniteStateMachine::Sum(const FiniteStateMachine& machine) {
    if (terminated_state_.size() != 1 || machine.terminated_state_.size() != 1) {
        throw std::runtime_error("for catcatination need machines with 1 tinish state");
    }

    if (terminated_state_[0] == initial_state_) {
        throw std::runtime_error("can't sum machines, where initial and terminate states is same");
    }

    if (machine.terminated_state_[0] == machine.initial_state_) {
        throw std::runtime_error("can't sum machines, where initial and terminate states is same");
    }

    MergeAlphabet(machine.sigma_);

    size_t shift = states_.size();
    delta_.resize(states_.size() + machine.states_.size());
    for (const auto& state : machine.states_) {
        if (machine.initial_state_ != state && machine.terminated_state_[0] != state) {
            states_.push_back(state + shift);	
        }
    }

    for (size_t state = 0; state < machine.delta_.size(); ++state) {
        for (const auto& [symbol, next_states]: machine.delta_[state]) {
            for (const auto& next_state: next_states) {
                size_t v = state + shift;
                size_t u = next_state + shift;
                if (state == machine.initial_state_) {
                    v = initial_state_;
                }
                if (state == machine.terminated_state_[0]) {
                    v = terminated_state_[0];
                }

                if (next_state == machine.terminated_state_[0]) {
                    u = terminated_state_[0];
                }
                if (next_state == machine.initial_state_) {
                    u = initial_state_;
                }

                delta_[v][symbol].push_back(u);
            }
        }
    }
    Zip();
}

void FiniteStateMachine::MakeCircle() {
    if (terminated_state_.size() != 1) {
        throw std::runtime_error("for catcatination need machines with 1 tinish state");
    }

    delta_[terminated_state_[0]][eps].push_back(initial_state_);
    terminated_state_[0] = initial_state_;
}

void FiniteStateMachine::MergeAlphabet(const std::vector<char>& alphabet) {
    for (char c: alphabet) {
        sigma_.push_back(c);
    }
    std::sort(sigma_.begin(), sigma_.end());
    sigma_.erase(std::unique(sigma_.begin(), sigma_.end()), sigma_.end());
}
 

bool FiniteStateMachine::dfs(size_t current_state, const std::string& word, size_t id) {
    if (id == word.size() && IsTerminate(current_state)) {
        return true;
    }
    if (delta_[current_state].find(word[id]) != delta_[current_state].end()) {
        for (const auto& next_state: delta_[current_state][word[id]]) {
            if (dfs(next_state, word, id + 1)) {
                return true;
            }
        }
    }
    if (delta_[current_state].find(eps) != delta_[current_state].end()) {
        for (const auto& next_state: delta_[current_state][eps]) {
            if (dfs(next_state, word, id)) {
                return true;
            }
        }
    }
    return false;
}

bool FiniteStateMachine::IsTerminate(size_t state) const {
    for (const auto& terminate_state: terminated_state_) {
        if (terminate_state == state) {
            return true;
        }
    }
    return false;
}

void FiniteStateMachine::Expand() {
    for (auto& state: states_) {
        state = 1 << state;
    }

    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta(1 << delta_.size());
    for (size_t state = 0; state < delta_.size(); ++state) {
        for (auto& iter: delta_[state]) {
            auto& symbol = iter.first;
            auto& next_states = iter.second;
            for (auto& next_state: next_states) {
                new_delta[1 << state][symbol].push_back(1 << next_state);
            }
        }
    }	
    delta_ = std::move(new_delta);

    initial_state_ = 1 << initial_state_;
    for (auto& state: terminated_state_) {
        state = 1 << state;
    }
}

std::vector<std::vector<size_t>> FiniteStateMachine::BuildEpsClosure() {
    std::vector<std::vector<size_t>> eps_closure(delta_.size());
    for (auto& state: states_) {
        std::queue<size_t> buffer;
        std::unordered_set<size_t> was_in;
        buffer.push(state);
        while (!buffer.empty()) {
            auto current_state = buffer.front();
            buffer.pop();
            if (was_in.find(current_state) != was_in.end()) {
                continue;
            } 
            was_in.insert(current_state);
            eps_closure[state].push_back(current_state);
            for (auto& next_state: delta_[current_state][eps]) {
                buffer.push(next_state);
            }	
        }
    }
    return eps_closure;
}

size_t FiniteStateMachine::UniqueStates(const std::vector<size_t>& states) {
    size_t uniqued_state = 0;
    for (auto& state: states) {
        uniqued_state ^= state;
    }
    return uniqued_state;
}

size_t FiniteStateMachine::ExpandAndUniqueStates(const std::vector<size_t>& states) {
    size_t uniqued_state = 0;
    for (auto& state: states) {
        uniqued_state ^= (1 << state);
    }
    return uniqued_state;
}

void FiniteStateMachine::Zip() {
    std::sort(states_.begin(), states_.end());
    std::unordered_map<size_t, size_t> dictinory;
    for (size_t i = 0; i < states_.size(); ++i) {
        dictinory[states_[i]] = i;
    }

    for (auto& state: states_) {
        state = dictinory[state];
    }

    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta(states_.size());
    for (size_t state = 0; state < delta_.size(); ++state) {
        for (auto& [symbol, next_states]: delta_[state]) {
            for (auto& next_state: next_states) {
                new_delta[dictinory[state]][symbol].push_back(dictinory[next_state]);
            }
        }
    }	
    delta_ = std::move(new_delta);

    initial_state_ = dictinory[initial_state_];
    for (auto& state: terminated_state_) {
        state = dictinory[state];
    }
    std::sort(terminated_state_.begin(), terminated_state_.end());
}


