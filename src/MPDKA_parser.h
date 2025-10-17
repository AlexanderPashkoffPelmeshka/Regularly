#pragma once

class MPDKA_Builder {
    std::vector<char> alphabets;
    bool kill_optimisation = true;

    size_t find_class(const std::vector<std::set<size_t>>& partition, size_t state_id) {
        for (size_t i = 0; i < partition.size(); ++i) {
            if (partition[i].find(state_id) != partition[i].end()) {
                return i;
            }
        }
        throw std::runtime_error("State not found in partition");
    }

public:
    MPDKA_Builder(std::vector<char>&& another) : alphabets(std::move(another)) {}
    MPDKA_Builder(const std::vector<char>& another) : alphabets(another) {}

    void set_kill_optimisation(bool val) noexcept { kill_optimisation = val; }

    DFA build(const DFA& dfa) {
        std::vector<size_t> all_states;
        for (size_t i = 0; i < dfa.get_states().size(); ++i) {
            all_states.push_back(i);
        }

        std::set<size_t> final_states;
        std::set<size_t> non_final_states;
        
        for (size_t state_id = 0; state_id < dfa.get_states().size(); ++state_id) {
            if (dfa.get_state(state_id)->is_finish()) {
                final_states.insert(state_id);
            } else {
                non_final_states.insert(state_id);
            }
        }

        std::vector<std::set<size_t>> partition;
        if (!final_states.empty()) partition.push_back(final_states);
        if (!non_final_states.empty()) partition.push_back(non_final_states);

        bool changed = true;
        while (changed) {
            changed = false;
            std::vector<std::set<size_t>> new_partition;

            for (const auto& group : partition) {
                if (group.size() == 1) {
                    new_partition.push_back(group);
                    continue;
                }

                std::vector<std::set<size_t>> subgroups;
                
                for (size_t state_id : group) {
                    bool placed = false;
                    
                    for (auto& subgroup : subgroups) {
                        size_t representative = *subgroup.begin();
                        bool equivalent = true;
                        for (char symbol : alphabets) {
                            const auto& state_links = dfa.get_state(state_id)->get_links();
                            const auto& repr_links = dfa.get_state(representative)->get_links();
                            
                            auto state_it = state_links.find(symbol);
                            auto repr_it = repr_links.find(symbol);

                            if (state_it != state_links.end() && repr_it != repr_links.end()) {
                                size_t state_target = state_it->second;
                                size_t repr_target = repr_it->second;

                                if (find_class(partition, state_target) != find_class(partition, repr_target)) {
                                    equivalent = false;
                                    break;
                                }
                            } 
                            else if (state_it != state_links.end() || repr_it != repr_links.end()) {
                                equivalent = false;
                                break;
                            }
                        }
                        
                        if (equivalent) {
                            subgroup.insert(state_id);
                            placed = true;
                            break;
                        }
                    }
                    
                    if (!placed) {
                        subgroups.push_back({state_id});
                    }
                }
                for (const auto& subgroup : subgroups) {
                    new_partition.push_back(subgroup);
                }
            }
            if (new_partition.size() != partition.size()) {
                changed = true;
            } else {
                for (size_t i = 0; i < partition.size(); ++i) {
                    if (partition[i] != new_partition[i]) {
                        changed = true;
                        break;
                    }
                }
            }
            
            partition = new_partition;
        }
        return build_minimal_dfa(dfa, partition);
    }

private:
    DFA build_minimal_dfa(const DFA& original_dfa, const std::vector<std::set<size_t>>& partition) {
        std::map<size_t, size_t> old_to_new;
        for (size_t new_id = 0; new_id < partition.size(); ++new_id) {
            for (size_t old_id : partition[new_id]) {
                old_to_new[old_id] = new_id;
            }
        }
        DFA min_dfa(0);
        for (size_t i = 0; i < partition.size(); ++i) {
            DFA_State* new_state = min_dfa.add_state(i);
            size_t representative = *partition[i].begin();
            if (original_dfa.get_state(representative)->is_finish()) {
                new_state->set_finish(true);
                min_dfa.add_final_state(i);
            }
        }

        size_t old_start = original_dfa.get_start_state();
        size_t new_start = old_to_new[old_start];
        min_dfa.set_start_state(new_start);

        for (size_t i = 0; i < partition.size(); ++i) {
            DFA_State* new_state = min_dfa.get_state(i);

            size_t representative = *partition[i].begin();
            const auto& old_links = original_dfa.get_state(representative)->get_links();
            
            for (char symbol : alphabets) {
                auto it = old_links.find(symbol);
                if (it != old_links.end()) {
                    size_t old_target = it->second;
                    size_t new_target = old_to_new[old_target];
                    new_state->add_link(symbol, new_target);
                }
            }
        }

        if (kill_optimisation) {
          killed_optimisation(min_dfa);
        }
        
        return min_dfa;
    }
};