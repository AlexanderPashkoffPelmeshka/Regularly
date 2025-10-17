#pragma once

class DFA_State {
 private:
  std::unordered_map<char, size_t> links;
  size_t id;
  bool is_finished = false;
  bool kill_optimisation = false;

 public:
  DFA_State(size_t state_id) : id(state_id) {}

  void add_link(char c, size_t target_state) {
    links[c] = target_state;
  }

  bool is_finish() const noexcept { return is_finished; }
  void set_finish(bool val) noexcept { is_finished = val; }
  size_t get_id() const noexcept { return id; }
  const auto& get_links() const noexcept { return links; }

  std::optional<size_t> get_neighbour(char c) const noexcept { 
    auto it = links.find(c);
    if (it != links.end()) {
      return it->second;
    } else {
      return std::nullopt;
    }
  }

  size_t get_kill_optimisation() const noexcept { return kill_optimisation; }

  void set_kill_optimisation(bool val) noexcept { kill_optimisation = val; }

  bool is_closed() {
    for (auto&& [key, value] : links) {
      if (value != id) return false;
    }
    return true;
  }
};

class DFA {
 private:
  std::vector<DFA_State*> states;
  size_t start_state;
  std::vector<size_t> final_states;

 public:
  DFA(size_t start) : start_state(start) {}
  
  DFA(const DFA&) = delete;
  DFA& operator=(const DFA&) = delete;
  
  DFA(DFA&& another) noexcept : 
    states(std::move(another.states)), 
    start_state(another.start_state),
    final_states(std::move(another.final_states)) {
  }

  ~DFA() {
    for (auto state : states) {
      delete state;
    }
  }

  DFA_State* add_state(size_t id) {
    auto new_state = new DFA_State(id);
    states.push_back(new_state);
    return new_state;
  }

  DFA_State* get_state(size_t id) const {
    if (id < states.size()) {
      return states[id];
    }
    return nullptr;
  }

  void set_start_state(size_t start) noexcept { start_state = start; }
  void add_final_state(size_t state_id) { final_states.push_back(state_id); }

  size_t get_start_state() const noexcept { return start_state; }
  const auto& get_final_states() const noexcept { return final_states; }
  const auto& get_states() const noexcept { return states; }
  size_t get_states_count() const noexcept { return states.size(); }

  void print_DFA() {
    std::cout << std::format("Начальное состояние: {}\n", start_state);
    std::cout << "Конечные состояния: ";
    for (const auto& x : final_states) std::cout << x << "  ";
    std::cout << "\n";
    
    for (const auto& state : states) {
      for (const auto& [symbol, target] : state->get_links()) {
        std::cout << std::format("({}{}) --{}--> ({})\n", 
          state->get_id(), (state->get_kill_optimisation())? '`': '\0',
          symbol, target);
      }
    }
  }
  
  int Task_15(const std::string& str) {
    int result = 0;
    char let;
    DFA_State* pointer = states[start_state];
    for (size_t i = 0; i < str.size(); ++i) {
      if (pointer->get_neighbour(str[i]).has_value()) {
        pointer = states[pointer->get_neighbour(str[i]).value()];
        if (pointer->is_finish()) {
          result = i + 1;
        } else if (pointer->get_kill_optimisation()) {
          return result;
        }
      } else {
        return result;
      }
    }
    return result;
  }
};