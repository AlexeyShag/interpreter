
#include <finite_state_machine/finite_state_machine.h>

std::string ParseSymbol(char c) {
	if ('a' <= c && 'z' >= c) {
		return std::string(1, c);
	} else {
		return "eps";
	}
}

FiniteStateMachine& FiniteStateMachine::SetNumberState(size_t n) {
    q_.resize(n);
    delta_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        q_[i] = i;
    }
    return *this;
}

FiniteStateMachine& FiniteStateMachine::SetInitialState(size_t i) {
    i_ = i;
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
    return q_.size();
}

size_t FiniteStateMachine::GetNumberTerminateState() const {
    return f_.size();
}
	
FiniteStateMachine& FiniteStateMachine::AddNewState(size_t new_state) {
    if (delta_.size() > new_state && delta_[new_state].size() != 0) {
        throw std::runtime_error("This state is already exists");
    }
    delta_.resize(std::max(delta_.size(), new_state + 1));
    q_.push_back(new_state);
    return *this;
}

FiniteStateMachine& FiniteStateMachine::AddNewState(size_t new_state, std::unordered_map<char, std::vector<size_t>> transition) {
    AddNewState(new_state);
    delta_[new_state] = std::move(transition);
    return *this;
}


void FiniteStateMachine::Clear() {
    i_ = 0;
    delta_.clear();
    q_.clear();
    f_.clear();
    sigma_.clear();
}

FiniteStateMachine& FiniteStateMachine::AddTerminated(size_t state) {
    f_.push_back(state);
    return *this;
}

const std::unordered_map<char, std::vector<size_t>>& FiniteStateMachine::operator[](size_t i) const {
    return delta_[i];
}
	

bool FiniteStateMachine::CheckingWord(const std::string& s) {
    return dfs(i_, s, 0);
}

size_t FiniteStateMachine::GetInitialState() const {
    return i_;
}

void FiniteStateMachine::DeleteEmptyTransitions() {
    Expand();
    auto eps_closure = BuildEpsClosure();
    
    size_t new_i = UniqueStates(eps_closure[i_]);
    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta (1 << q_.size());
    std::vector<size_t> new_q;
    size_t f = UniqueStates(f_);
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
            for (size_t state_id = 0; state_id < q_.size(); ++state_id) {
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
    i_ = new_i;
    q_ = std::move(new_q);
    delta_ = std::move(new_delta);
    f_ = std::move(new_f);

    Zip();
}

void FiniteStateMachine::MakeDeterministic() {
    Expand();

    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta(1 << q_.size());
    std::vector<size_t> new_q;
    size_t f = UniqueStates(f_);
    std::vector<size_t> new_f;

    std::queue<size_t> buffer;
    std::unordered_set<size_t> was_in;
    buffer.push(i_);
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
            for (size_t state_id = 0; state_id < q_.size(); ++state_id) {
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

    q_ = std::move(new_q);
    delta_ = std::move(new_delta);
    f_ = std::move(new_f);

    Zip();
}

void FiniteStateMachine::MakeFullDeterministic() {
    size_t new_state = q_.size();
    bool is_new_state_created = false;
    for (auto& state: q_) {
        for (auto symbol: sigma_) {
            if (delta_[state].find(symbol) == delta_[state].end()) {
                delta_[state][symbol].push_back(new_state);
                is_new_state_created = true;
            }
        }
    }
    if (is_new_state_created) {	
        delta_.resize(q_.size() + 1);
        for (auto symbol: sigma_) {
            delta_[new_state][symbol].push_back(new_state);
        }
        q_.push_back(new_state);
    }
}	

void FiniteStateMachine::MakeAddition() {
    size_t uniqued_t = ExpandAndUniqueStates(f_);
    f_.clear();
    for (size_t state_id = 0; state_id < q_.size(); ++state_id) {
        if (!(uniqued_t & (1 << state_id))) {
            f_.push_back(state_id);
        }
    }
}

void FiniteStateMachine::MakeMinFullDeterministic() {
    Expand();
    std::vector<size_t> classes((1 << q_.size()), 1);
    for (auto& state: f_) {
        classes[state] = 2;
    }
    size_t number_classes = 2;
    size_t last_number_classes = -1;
    while (number_classes != last_number_classes) {
        last_number_classes = number_classes;
        std::map<std::vector<size_t>, std::vector<size_t>> new_classes;
        for (auto& current_state: q_) {
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
    i_ = classes[i_];

    q_.resize(number_classes);
    for (size_t i = 0; i < q_.size(); ++i) {
        q_[i] = (1 << i);
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

    for(auto& state: f_) {
        state = classes[state];
    }
    std::sort(f_.begin(), f_.end());
    f_.erase(std::unique(f_.begin(), f_.end()), f_.end());

    Zip();
}	

void FiniteStateMachine::print() const {
    std::cerr << "State:\n";
    for (auto& state: q_) {
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
    std::cerr << i_ << '\n';
    std::cerr << "\nTerminate:\n";
    for (auto& state: f_) {
        std::cerr << state << ' ';
    }
    std::cerr << '\n';

    std::cerr << "================\n";

}

void FiniteStateMachine::Cat(const FiniteStateMachine& machine) {
    if (f_.size() != 1) {
        throw std::runtime_error("for catcatination need machines with 1 tinish state");
    }

    MergeAlphabet(machine.sigma_);

    size_t shift = q_.size();
    delta_.resize(q_.size() + machine.q_.size());

    for (const auto& state : machine.q_) {
        if (state != machine.i_) {
            q_.push_back(state + shift);	
        }
    }

    for (size_t state = 0; state < machine.delta_.size(); ++state) {
        for (const auto& [symbol, next_states]: machine.delta_[state]) {
            for (const auto& next_state: next_states) {
                size_t v = state + shift;
                size_t u = next_state + shift;
                if (state == machine.i_) {
                    v = f_[0];
                }
                if (next_state == machine.i_) {
                    u = f_[0];
                }
                delta_[v][symbol].push_back(u);
            }
        }
    }

    if (machine.i_ != machine.f_[0]) {
        f_ = machine.f_;
        for (auto& state: f_) {
            state += shift;
        }
    }

    Zip();
}

void FiniteStateMachine::Sum(const FiniteStateMachine& machine) {
    if (f_.size() != 1 || machine.f_.size() != 1) {
        throw std::runtime_error("for catcatination need machines with 1 tinish state");
    }

    if (f_[0] == i_) {
        throw std::runtime_error("can't sum machines, where initial and terminate states is same");
    }

    if (machine.f_[0] == machine.i_) {
        throw std::runtime_error("can't sum machines, where initial and terminate states is same");
    }

    MergeAlphabet(machine.sigma_);

    size_t shift = q_.size();
    delta_.resize(q_.size() + machine.q_.size());
    for (const auto& state : machine.q_) {
        if (machine.i_ != state && machine.f_[0] != state) {
            q_.push_back(state + shift);	
        }
    }

    for (size_t state = 0; state < machine.delta_.size(); ++state) {
        for (const auto& [symbol, next_states]: machine.delta_[state]) {
            for (const auto& next_state: next_states) {
                size_t v = state + shift;
                size_t u = next_state + shift;
                if (state == machine.i_) {
                    v = i_;
                }
                if (state == machine.f_[0]) {
                    v = f_[0];
                }

                if (next_state == machine.f_[0]) {
                    u = f_[0];
                }
                if (next_state == machine.i_) {
                    u = i_;
                }

                delta_[v][symbol].push_back(u);
            }
        }
    }
    Zip();
}

void FiniteStateMachine::MakeCircle() {
    if (f_.size() != 1) {
        throw std::runtime_error("for catcatination need machines with 1 tinish state");
    }

    delta_[f_[0]][eps].push_back(i_);
    f_[0] = i_;
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
    for (const auto& terminate_state: f_) {
        if (terminate_state == state) {
            return true;
        }
    }
    return false;
}

void FiniteStateMachine::Expand() {
    for (auto& state: q_) {
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

    i_ = 1 << i_;
    for (auto& state: f_) {
        state = 1 << state;
    }
}

std::vector<std::vector<size_t>> FiniteStateMachine::BuildEpsClosure() {
    std::vector<std::vector<size_t>> eps_closure(delta_.size());
    for (auto& state: q_) {
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
    std::sort(q_.begin(), q_.end());
    std::unordered_map<size_t, size_t> dictinory;
    for (size_t i = 0; i < q_.size(); ++i) {
        dictinory[q_[i]] = i;
    }

    for (auto& state: q_) {
        state = dictinory[state];
    }

    std::vector<std::unordered_map<char, std::vector<size_t>>> new_delta(q_.size());
    for (size_t state = 0; state < delta_.size(); ++state) {
        for (auto& [symbol, next_states]: delta_[state]) {
            for (auto& next_state: next_states) {
                new_delta[dictinory[state]][symbol].push_back(dictinory[next_state]);
            }
        }
    }	
    delta_ = std::move(new_delta);

    i_ = dictinory[i_];
    for (auto& state: f_) {
        state = dictinory[state];
    }
    std::sort(f_.begin(), f_.end());
}


