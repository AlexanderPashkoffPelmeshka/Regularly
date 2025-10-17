#pragma once

class PDKA_Builder {
private:
    std::vector<char> alphabets;
    std::set<size_t> epsilon_closure(const Tree& nka, const std::set<size_t>& states) {
        std::set<size_t> closure = states;
        std::stack<size_t> stack;
        
        for (size_t state_id : states) {
            stack.push(state_id);
        }

        while (!stack.empty()) {
            size_t current_id = stack.top();
            stack.pop();
            
            State* current_state = nka.buffer[current_id];
            auto links = current_state->get_links();
            if (links.find('\0') != links.end()) {
                for (State* next_state : links.at('\0')) {
                    size_t next_id = next_state->get_id();
                    if (closure.find(next_id) == closure.end()) {
                        closure.insert(next_id);
                        stack.push(next_id);
                    }
                }
            }
        }
        
        return closure;
    }
    std::set<size_t> move(const Tree& nka, const std::set<size_t>& states, char symbol) {
        std::set<size_t> result;
        
        for (size_t state_id : states) {
            State* current_state = nka.buffer[state_id];
            auto links = current_state->get_links();
            
            if (links.find(symbol) != links.end()) {
                for (State* next_state : links.at(symbol)) {
                    result.insert(next_state->get_id());
                }
            }
        }
        
        return result;
    }

    void make_complete_dfa(DFA& dfa) {
        bool need_dead_state = false;
        
        for (size_t i = 0; i < dfa.get_states_count(); ++i) {
            DFA_State* state = dfa.get_state(i);
            for (char symbol : alphabets) {
                if (state->get_links().find(symbol) == state->get_links().end()) {
                    need_dead_state = true;
                    break;
                }
            }
            if (need_dead_state) break;
        }

        if (!need_dead_state) {
            return;
        }

        size_t dead_state_id = dfa.get_states_count();
        DFA_State* dead_state = dfa.add_state(dead_state_id);

        for (char symbol : alphabets) {
            dead_state->add_link(symbol, dead_state_id);
        }

        for (size_t i = 0; i < dfa.get_states_count() - 1; ++i) {
            DFA_State* state = dfa.get_state(i);
            for (char symbol : alphabets) {
                if (state->get_links().find(symbol) == state->get_links().end()) {
                    state->add_link(symbol, dead_state_id);
                }
            }
        }
    }

public:
    PDKA_Builder(std::vector<char>&& another) : alphabets(std::move(another)) {}
    PDKA_Builder(const std::vector<char>& another) : alphabets(another) {}

    DFA build(const Tree& nka) {
        std::map<std::set<size_t>, size_t> state_map;
        std::queue<std::set<size_t>> unprocessed_states;
        DFA dfa(0);

        std::set<size_t> start_set = epsilon_closure(nka, {nka.start});
        state_map[start_set] = 0;
        unprocessed_states.push(start_set);
        
        DFA_State* start_state = dfa.add_state(0);

        for (size_t state_id : start_set) {
            if (nka.buffer[state_id]->is_finish()) {
                start_state->set_finish(true);
                dfa.add_final_state(0);
                break;
            }
        }

        size_t next_state_id = 1;
        while (!unprocessed_states.empty()) {
            std::set<size_t> current_set = unprocessed_states.front();
            unprocessed_states.pop();
            
            size_t current_dfa_state_id = state_map[current_set];
            DFA_State* current_dfa_state = dfa.get_state(current_dfa_state_id);

            for (char symbol : alphabets) {
                if (symbol == '\0') continue;

                std::set<size_t> next_set = epsilon_closure(nka, move(nka, current_set, symbol));
                
                if (next_set.empty()) continue;

                if (state_map.find(next_set) == state_map.end()) {
                    state_map[next_set] = next_state_id;
                    unprocessed_states.push(next_set);
                    
                    DFA_State* new_dfa_state = dfa.add_state(next_state_id);
                    for (size_t state_id : next_set) {
                        if (nka.buffer[state_id]->is_finish()) {
                            new_dfa_state->set_finish(true);
                            dfa.add_final_state(next_state_id);
                            break;
                        }
                    }
                    
                    current_dfa_state->add_link(symbol, next_state_id);
                    next_state_id++;
                } else {
                    size_t existing_state_id = state_map[next_set];
                    current_dfa_state->add_link(symbol, existing_state_id);
                }
            }
        }
        make_complete_dfa(dfa);

        return std::move(dfa);
    }
};