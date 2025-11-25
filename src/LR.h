#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <stack>
#include <queue>
#include <memory>
#include <optional>
#include <set>
#include <iomanip>
#include <algorithm>

class Grammar {
  std::map<char, std::vector<std::vector<char>>> rules;
  std::set<char> non_terminals = {'\x1F'};
  std::set<char> terminals = {'\x1E', '\x1D'};
  char start_symbol = '\0';

  std::map<char, std::set<char>> first;

  std::set<char> compute_sequence_first(const std::vector<char>& sequence) const {
    std::set<char> result;
    bool can_produce_epsilon = true;

    for (char symbol : sequence) {
      if (!can_produce_epsilon) break;

      if (terminals.count(symbol)) {
        if (symbol != '\x1D') {
          result.insert(symbol);
          can_produce_epsilon = false;
        } else {
          continue;
        }
      } else {
        auto it = first.find(symbol);
        if (it != first.end()) {
          const std::set<char>& first_of_symbol = it->second;
          bool has_epsilon = (first_of_symbol.find('\x1D') != first_of_symbol.end());

          for (char s : first_of_symbol) {
            if (s != '\x1D') {
              result.insert(s);
            }
          }

          can_produce_epsilon = has_epsilon;
        }
      }
    }

    if (can_produce_epsilon) {
      result.insert('\x1D');
    }

    return result;
  }

 public:

  void set_start_symbol(char s) { 
    start_symbol = s; 
  }
  
  char get_start_symbol() const { 
    if (start_symbol == '\0') {
      throw std::invalid_argument("НЕ УСТАНОВЛЕН СТАРТОВЫЙ НеТеРмиНаЛ");
    }
    return start_symbol; 
  }

  void set_terminals(const std::vector<char>& other) {
    if (other.empty()) throw std::invalid_argument("Пустой алфавит");
    terminals.clear();
    terminals.insert('\x1E');
    terminals.insert('\x1D');
    for (auto&& x : other) {
      terminals.insert(x);
    }
  }
  
  void set_neterminals(const std::vector<char>& other) {
    if (other.empty()) throw std::invalid_argument("Пустой массив нетерминалов");
    non_terminals.clear();
    non_terminals.insert('\x1F');
    for (auto&& x : other) {
      non_terminals.insert(x);
    }
  }
  
  void clear() {
    first.clear();
    rules.clear();
    non_terminals = {'\x1F'};
    terminals = {'\x1E'};
    start_symbol = '\0';
  }

  const std::map<char, std::vector<std::vector<char>>>& get_rules() const {
    return rules;
  }

  const std::set<char>& get_terminals() const {
    return terminals;
  }

  const std::set<char>& get_non_terminals() const {
    return non_terminals;
  }

  void add_rule(char non_terminal, const std::vector<char>& production) {
    if (non_terminals.count(non_terminal) == 0) {
      throw std::invalid_argument("Неизвестный нетерминал: " + std::string(1, non_terminal));
    }

    for (char symbol : production) {
      if (symbol != '\x1D' &&
        terminals.count(symbol) == 0 && 
        non_terminals.count(symbol) == 0) {
        throw std::invalid_argument("Неизвестный символ в правиле: " + std::string(1, symbol));
      }
    }
    rules[non_terminal].push_back(production);
    non_terminals.insert(non_terminal);
  }

  void calculate_first() {
    first.clear();
    for (const auto& term : terminals) {
      if (term != '\x1D') {
        first[term].insert(term);
      }
    }

    if (terminals.count('\x1D')) {
      first['\x1D'].insert('\x1D');
    }

    for (const auto& nt : non_terminals) {
      first[nt];
    }

    bool changed;
    do {
      changed = false;
      for (const auto& rule_pair : rules) {
        char A = rule_pair.first;
        for (const auto& production : rule_pair.second) {
          std::set<char> new_first = compute_sequence_first(production);
          for (char symbol : new_first) {
            if (first[A].insert(symbol).second) {
              changed = true;
            }
          }
        }
      }
    } while (changed);
  }

  std::set<char> get_first(const std::vector<char>& sequence) const {
    return compute_sequence_first(sequence);
  }

  void print_first() {
    for (char nt : non_terminals) {
      std::cout << "FIRST(" << nt << ") = { ";
      for (char symbol : first[nt]) {
        std::cout << symbol << " ";
      }
      std::cout << "}\n";
    }
  }
};

class Rule {
  std::vector<char> rule;
  size_t point = 0;
  char main_symbol;
  char lookahead;

 public:
  Rule(const std::vector<char>& other, char la = '\0') {
    if (other.empty()) {
      main_symbol = '\0';
      lookahead = la;
    } else {
      main_symbol = other[0];
      rule = std::vector<char>(other.begin() + 1, other.end());
      lookahead = la;
    }
  }

  Rule() : main_symbol('\0'), lookahead('\0') {};
  Rule(const Rule& other) = default;
  Rule& operator=(const Rule& other) = default;
  
  void swap(Rule& tmp) {
    std::swap(rule, tmp.rule);
    std::swap(point, tmp.point);
    std::swap(main_symbol, tmp.main_symbol);
    std::swap(lookahead, tmp.lookahead);
  }
  

  char get_main() const { return main_symbol; }
  char get_lookahead() const { return lookahead; }
  const std::vector<char>& get_production() const { return rule; }
  std::vector<char>& get_production() { return rule; }
  size_t get_point() const { return point; }
  
  void point_inc() {
    if (point < rule.size()) {
      ++point; 
    }
  }
  
  char symbol_after_point() const { 
    return (point < rule.size()) ? rule[point] : '\0'; 
  }
  
  std::vector<char> symbols_after_point() const {
    if (point >= rule.size()) return {};
    return std::vector<char>(rule.begin() + point, rule.end());
  }
};

class State {
  std::unordered_map<char, std::vector<Rule>> table;
  std::set<std::pair<char, size_t>> links;
  size_t id;

  bool has_rule(const std::vector<Rule>& rules, const Rule& new_rule) {
    for (const auto& rule : rules) {
      if (rule.get_main() == new_rule.get_main() &&
          rule.get_production() == new_rule.get_production() &&
          rule.get_point() == new_rule.get_point() &&
          rule.get_lookahead() == new_rule.get_lookahead()) {
        return true;
      }
    }
    return false;
  }

 public:
  State(int32_t id) : id(id) { }

  size_t get_id() const { return id; }

  void add_link(const std::pair<char, size_t>& link) {
    links.insert(link);
  }

  const std::unordered_map<char, std::vector<Rule>>& get_table() const {
    return table;
  }

  const std::set<std::pair<char, size_t>>& get_links() const {
    return links;
  }

  std::vector<std::pair<char, std::vector<Rule>>> get_all_rules_grouped() const {
    std::vector<std::pair<char, std::vector<Rule>>> result;
    for (const auto& pair : table) {
      result.emplace_back(pair.first, pair.second);
    }
    return result;
  }

  std::vector<Rule> get_all_rules() const {
    std::vector<Rule> all_rules;
    for (const auto& pair : table) {
      all_rules.insert(all_rules.end(), pair.second.begin(), pair.second.end());
    }
    return all_rules;
  }

  std::optional<std::vector<Rule>> get_rules_for_letter(char letter) const {
    auto it = table.find(letter);
    if (it == table.end()) return std::nullopt;
    return it->second;
  }

  void add_rule(const Rule& rule) {
    char symbol_after_point = rule.symbol_after_point();
    char storage_symbol = (symbol_after_point == '\0') ? '.' : symbol_after_point;
    
    if (!has_rule(table[storage_symbol], rule)) {
      table[storage_symbol].push_back(rule);
    }
  }

  void add_link(char c, size_t id) {
    links.insert({c, id});
  }

  void transitive_closure(const Grammar& grammar) {
    bool changed;
    do {
      changed = false;
      
      std::vector<Rule> all_rules = get_all_rules();
      
      for (const Rule& rule : all_rules) {
        char next_symbol = rule.symbol_after_point();
        
        if (next_symbol != '\0' && grammar.get_non_terminals().count(next_symbol)) {
          std::vector<char> beta = rule.symbols_after_point();
          if (!beta.empty()) beta.erase(beta.begin());
          beta.push_back(rule.get_lookahead());
          
          std::set<char> first_beta_a = grammar.get_first(beta);
          
          const auto& rules_for_nt = grammar.get_rules();
          auto it = rules_for_nt.find(next_symbol);
          if (it != rules_for_nt.end()) {
            for (const auto& production : it->second) {
              if (production.empty() || (production.size() == 1 && production[0] == '\x1D')) {
                for (char lookahead : first_beta_a) {
                  if (lookahead == '\x1D') continue;

                  std::vector<char> new_rule_content = {next_symbol};
                  Rule new_rule(new_rule_content, lookahead);

                  char storage_sym = new_rule.symbol_after_point();
                  storage_sym = (storage_sym == '\0') ? '.' : storage_sym;
                  
                  if (!has_rule(table[storage_sym], new_rule)) {
                    table[storage_sym].push_back(new_rule);
                    changed = true;
                  }
                }
              } else {
                for (char lookahead : first_beta_a) {
                  if (lookahead == '\x1D') {
                    continue;
                  }

                  std::vector<char> new_rule_content = {next_symbol};
                  new_rule_content.insert(new_rule_content.end(), production.begin(), production.end());
                  Rule new_rule(new_rule_content, lookahead);

                  char storage_sym = new_rule.symbol_after_point();
                  storage_sym = (storage_sym == '\0') ? '.' : storage_sym;
                  
                  if (!has_rule(table[storage_sym], new_rule)) {
                    table[storage_sym].push_back(new_rule);
                    changed = true;
                  }
                }
              }
            }
          }
        }
      }
    } while (changed);
  }

  void print_state() const {
    std::cout << "Состояние " << id << ":\n";
    for (const auto& pair : table) {
      std::cout << "  Символ '" << pair.first << "':\n";
      for (const Rule& rule : pair.second) {
        std::cout << "    " << rule.get_main() << " -> ";
        const auto production = rule.get_production();
        for (size_t i = 0; i < production.size(); ++i) {
          if (i == rule.get_point()) std::cout << "• ";
          std::cout << production[i] << " ";
        }
        if (rule.get_point() == production.size()) std::cout << "•";
        std::cout << ", " << rule.get_lookahead() << "\n";
      }
    }

    if (!links.empty()) {
      std::cout << "  Переходы: ";
      for (const auto& link : links) {
        std::cout << link.first << "->" << link.second << " ";
      }
      std::cout << "\n";
    }
  }

  bool operator==(const State& other) const {
    if (table.size() != other.table.size()) {
      return false;
    }
    
    for (const auto& [symbol, rules] : table) {
      auto other_it = other.table.find(symbol);
      
      if (other_it == other.table.end()) {
        return false;
      }
      
      const auto& other_rules = other_it->second;
      
      if (rules.size() != other_rules.size()) {
        return false;
      }
      
      auto sorted_rules = rules;
      auto sorted_other_rules = other_rules;
      
      auto rule_less = [](const Rule& a, const Rule& b) {
        if (a.get_main() != b.get_main()) {
          return a.get_main() < b.get_main();
        }
        
        const auto& prod_a = a.get_production();
        const auto& prod_b = b.get_production();
        
        if (prod_a.size() != prod_b.size()) {
          return prod_a.size() < prod_b.size();
        }
        
        for (size_t i = 0; i < prod_a.size(); ++i) {
          if (prod_a[i] != prod_b[i]) {
            return prod_a[i] < prod_b[i];
          }
        }
        
        if (a.get_point() != b.get_point()) {
          return a.get_point() < b.get_point();
        }
        
        return a.get_lookahead() < b.get_lookahead();
      };
      
      std::sort(sorted_rules.begin(), sorted_rules.end(), rule_less);
      std::sort(sorted_other_rules.begin(), sorted_other_rules.end(), rule_less);
      
      for (size_t i = 0; i < sorted_rules.size(); ++i) {
        const Rule& rule1 = sorted_rules[i];
        const Rule& rule2 = sorted_other_rules[i];
        
        if (rule1.get_main() != rule2.get_main()) {
          return false;
        }
        
        const auto& prod1 = rule1.get_production();
        const auto& prod2 = rule2.get_production();
        
        if (prod1.size() != prod2.size()) {
          return false;
        }
        
        for (size_t j = 0; j < prod1.size(); ++j) {
          if (prod1[j] != prod2[j]) {
            return false;
          }
        }
        
        if (rule1.get_point() != rule2.get_point()) {
          return false;
        }
        
        if (rule1.get_lookahead() != rule2.get_lookahead()) {
          return false;
        }
      }
    }
    
    return true;
  }

  bool operator!=(const State& other) const {
    return !(*this == other);
  }
};

class Table_cell {
public:
  enum class Act {
    ERROR = 0,
    SHIFT = 1,
    REDUCE = 2,
    ACCEPT = 3,
    GOTO = 4
  };
  
  Act type = Act::ERROR;
  
  Table_cell() = default;
  explicit Table_cell(Act t) : type(t) {}
  virtual ~Table_cell() = default;
};

class SHIFT_cell : public Table_cell {
public:
  int32_t shift_id;
  SHIFT_cell(int32_t id) : Table_cell(Act::SHIFT), shift_id(id) {}
};

class REDUCE_cell : public Table_cell {
public:
  Rule rule;
  REDUCE_cell(const Rule& r) : Table_cell(Act::REDUCE), rule(r) {}
};

class ACCEPT_cell : public Table_cell {
public:
  ACCEPT_cell() : Table_cell(Act::ACCEPT) {}
};

class GOTO_cell : public Table_cell {
 public:
  int32_t goto_id;
  GOTO_cell(int32_t id) : Table_cell(Act::GOTO), goto_id(id) {}
};

class Action {
  const Grammar* grammar;
  std::unordered_map<char, std::vector<std::unique_ptr<Table_cell>>> table;

 public:
  Action(const Grammar* grammar, const std::vector<State>& vec_states, char c = '\x1F') : grammar(grammar) {
    if (!grammar) {
      throw std::invalid_argument("Где грамматика?");
    }

    size_t len = vec_states.size();
    if (len == 0) {
      throw std::invalid_argument("0 состояния для таблицы");
    }

    for (auto&& term : grammar->get_terminals()) {
      table[term] = std::vector<std::unique_ptr<Table_cell>>(len);
      for (size_t i = 0; i < len; ++i) {
          table[term][i] = std::make_unique<Table_cell>();
      }
    }
    for (auto&& non_term : grammar->get_non_terminals()) {
      table[non_term] = std::vector<std::unique_ptr<Table_cell>>(len);
      for (size_t i = 0; i < len; ++i) {
        table[non_term][i] = std::make_unique<Table_cell>();
      }
    }

    for (size_t i = 0; i < len; ++i) {
      const State& state = vec_states[i];

      std::map<char, int32_t> shift_actions;
      std::map<char, int32_t> goto_actions;
      
      for (auto&& [symbol, target_id] : state.get_links()) {
        if (grammar->get_terminals().count(symbol)) {
          if (shift_actions.count(symbol)) {
            throw std::invalid_argument("Ошибка в шифте");
          }
          shift_actions[symbol] = target_id;
        } else if (grammar->get_non_terminals().count(symbol)) {
          if (goto_actions.count(symbol)) {
            throw std::invalid_argument("Ошибка в шифте");
          }
          goto_actions[symbol] = target_id;
        }
      }

      std::map<char, Rule> reduce_actions;
      bool has_accept = false;

      auto dot_rules = state.get_rules_for_letter('.');
      if (dot_rules.has_value()) {
        for (auto&& rule : dot_rules.value()) {
          char lookahead = rule.get_lookahead();

          if (rule.get_main() == c && rule.get_production().size() == 1 &&
            rule.get_production()[0] == grammar->get_start_symbol() && 
            lookahead == '\x1E') {
            if (has_accept) {
              throw std::invalid_argument("Ошибка в accept");
            }
            has_accept = true;
        } else {
            if (reduce_actions.count(lookahead)) {
                throw std::invalid_argument("Ошибка в reduce");
            }
            reduce_actions[lookahead] = rule;
          }
        }
      }

      for (auto&& [symbol, _] : shift_actions) {
        if (reduce_actions.count(symbol)) {
          throw std::invalid_argument("Конфликт для символа");
        }
      }

      for (auto&& term : grammar->get_terminals()) {
        if (shift_actions.count(term)) {
          table[term][i] = std::make_unique<SHIFT_cell>(shift_actions[term]);
        } else if (reduce_actions.count(term)) {
          table[term][i] = std::make_unique<REDUCE_cell>(reduce_actions[term]);
        } else if (has_accept && term == '\x1E') {
          table[term][i] = std::make_unique<ACCEPT_cell>();
        }
      }
      for (auto&& non_term : grammar->get_non_terminals()) {
        if (goto_actions.count(non_term)) {
          table[non_term][i] = std::make_unique<GOTO_cell>(goto_actions[non_term]);
        }
      }
    }
  }

  const std::unordered_map<char, std::vector<std::unique_ptr<Table_cell>>>& get_table() const {
    return table;
  }

  void print_table() const {
    std::cout << "Таблица действий:\n";
    std::cout << "Состояние ";
    for (auto&& [symbol, _] : table) {
      std::cout << std::setw(8) << symbol;
    }
    std::cout << "\n";

    if (table.empty()) return;
    size_t num_states = table.begin()->second.size();

    for (size_t i = 0; i < num_states; ++i) {
      std::cout << std::setw(5) << i;
      for (auto&& [symbol, cells] : table) {
        std::cout << std::setw(8);
        switch (cells[i]->type) {
          case Table_cell::Act::SHIFT: {
            auto shift_cell = dynamic_cast<SHIFT_cell*>(cells[i].get());
            std::cout << "s" << shift_cell->shift_id;
            break;
          }
          case Table_cell::Act::REDUCE: {
            auto reduce_cell = dynamic_cast<REDUCE_cell*>(cells[i].get());
            std::cout << "r" << reduce_cell->rule.get_main();
            break;
          }
          case Table_cell::Act::ACCEPT:
            std::cout << "acc";
            break;
          case Table_cell::Act::GOTO: {
            auto goto_cell = dynamic_cast<GOTO_cell*>(cells[i].get());
            std::cout << "" << goto_cell->goto_id;
            break;
          }
          case Table_cell::Act::ERROR:
            std::cout << "";
            break;
        }
      }
      std::cout << "\n";
    }
  }
};

class Graph_State {
  std::vector<State> graph;

 public:
  Graph_State(const std::vector<State>& other = {}) : graph(other) {}

  void build_canonical_collection(const Grammar& grammar, const Rule& start_rule) {
    if (grammar.get_rules().empty()) {
        throw std::invalid_argument("Грамматика не содержит правил");
    }

    State I0(0);
    I0.add_rule(start_rule);
    I0.transitive_closure(grammar);
    graph.push_back(I0);

    size_t current_index = 0;

    while (current_index < graph.size()) {
      auto grouped_rules = graph[current_index].get_all_rules_grouped();

      for (auto& [symbol, rules] : grouped_rules) {
        if (symbol == '.') {
          continue;
        }

        State new_state(graph.size());
        for (auto& rule : rules) {
          Rule new_rule = rule;
          new_rule.point_inc();
          new_state.add_rule(new_rule);
        }

        new_state.transitive_closure(grammar);

        bool found = false;
        size_t existing_id = 0;

        for (const auto& existing_state : graph) {
          if (existing_state == new_state) {
            found = true;
            existing_id = existing_state.get_id();
            break;
          }
        }
        
        if (found) {
          graph[current_index].add_link(symbol, existing_id);
        } else {
          graph.push_back(new_state);
          graph[current_index].add_link(symbol, new_state.get_id());
        }
      }
      ++current_index;
    }
  }

  const std::vector<State>& get_graph() const {
    return graph;
  }

  void print_graph() const {
    std::cout << "LR(1):\n";
    std::cout << "Всего состояний: " << graph.size() << "\n";
    
    for (const auto& state : graph) {
      state.print_state();
      
      const auto& links = state.get_links();
      if (!links.empty()) {
        std::cout << "  Переходы:\n";
        for (const auto& [symbol, target_id] : links) {
          std::cout << "    '" << symbol << "' -> Состояние " << target_id << "\n";
        }
      }
      std::cout << "\n";
    }
    std::cout << "\n\n";
    for (auto&& x : graph) {
      x.print_state();
    }
  }
};

class LR_1_parser {
  Grammar grammar;
  std::unique_ptr<Action> action_table;
  bool is_fitted = false;
  
 public:
  LR_1_parser(const Grammar& g) : grammar(g) {}

  LR_1_parser() = default;

  Grammar& get_grammar() {
    return grammar;
  }

  void fit(bool logs = false) {
    if (grammar.get_rules().empty()) {
        throw std::invalid_argument("Грамматика не содержит правил");
    }
    
    if (grammar.get_non_terminals().size() <= 1) {
        throw std::invalid_argument("Не установлены нетерминалы");
    }
    
    if (grammar.get_terminals().size() <= 2) {
        throw std::invalid_argument("Не установлены терминалы");
    }


    Graph_State graph;
    std::vector<char> start_production = {'\x1F', grammar.get_start_symbol()};
    Rule start_rule(start_production, '\x1E');

    graph.build_canonical_collection(grammar, start_rule);

    action_table = std::make_unique<Action>(&grammar, graph.get_graph());
    
    is_fitted = true;
    if (logs) std::cout << "Всего состояний: " << graph.get_graph().size() << "\n";
  }

  bool parse(const std::vector<char>& input_word, bool logs = false) {
    if (!is_fitted) { throw std::invalid_argument("Вызови fit"); }
    
    if (!action_table) { throw std::invalid_argument("Таблицы нету"); }

    if (logs) {
      std::cout << "\nРазбор слова: ";
      for (char c : input_word) std::cout << c;
      std::cout << "\n";
    }

    std::stack<size_t> state_stack;
    state_stack.push(0);
    std::vector<char> input = input_word;
    input.push_back('\x1E');

    size_t input_index = 0;
    size_t step = 0;

    if (logs) {
      std::cout << "Шаг " << step << ": Стек состояний: [0], Вход: ";
      for (size_t i = input_index; i < input.size(); ++i) std::cout << input[i];
      std::cout << "\n";
    }

    while (true) {
      ++step;
      size_t current_state = state_stack.top();
      char current_symbol = input[input_index];

      const auto& table = action_table->get_table();
      auto it = table.find(current_symbol);
      
      if (it == table.end()) {
        if (logs) std::cout << "ОШИБКА: Неизвестный символ '" << current_symbol << "' во входной строке\n";
        throw std::invalid_argument("ОШИБКА");
        return false;
      }

      const auto& cell = it->second[current_state];
    
      switch (cell->type) {
        case Table_cell::Act::SHIFT: {
          auto shift_cell = dynamic_cast<SHIFT_cell*>(cell.get());
          if (!shift_cell) {
            if (logs) std::cout << "ОШИБКА в SHIFT\n";
            throw std::invalid_argument("ОШИБКА");
            return false;
          }

          state_stack.push(shift_cell->shift_id);
          ++input_index;
          if (logs) {
            std::cout << "Шаг " << step << ": СДВИГ в состояние " << shift_cell->shift_id << "\n";
            std::cout << "        Стек состояний: [";
            print_stack(state_stack);
            std::cout << "], Вход: ";
            for (size_t i = input_index; i < input.size(); ++i) std::cout << input[i];
            std::cout << "\n";
          }
          break;
        }
      
        case Table_cell::Act::REDUCE: {
          auto reduce_cell = dynamic_cast<REDUCE_cell*>(cell.get());
          if (!reduce_cell) {
            if (logs) std::cout << "ОШИБКА в REDUCE\n";
            throw std::invalid_argument("ОШИБКА в REDUCE");
            return false;
          }
          const Rule& rule = reduce_cell->rule;
          size_t pop_count = rule.get_production().size();
          if (pop_count == 1 && rule.get_production()[0] == '\x1D') {
            pop_count = 0;
          }

          if (state_stack.size() < pop_count) {
            if (logs) std::cout << "ОШИБКА в переполнении стека\n";
            throw std::invalid_argument("ОШИБКА в переполнении стека");
            return false;
          }

          for (size_t i = 0; i < pop_count; ++i) {
            state_stack.pop();
          }

          size_t new_state = state_stack.top();

          char non_terminal = rule.get_main();
          auto goto_it = table.find(non_terminal);
          if (goto_it == table.end()) {
            if (logs) std::cout << "ОШИБКА в GOTO'" << non_terminal << "'\n";
            throw std::invalid_argument("ОШИБКА в GOTO");
            return false;
          }
          
          const auto& goto_cell = goto_it->second[new_state];
          if (goto_cell->type != Table_cell::Act::GOTO) {
            if (logs) std::cout << "ОШИБКА в GOTO'" << non_terminal << "'\n";
            throw std::invalid_argument("ОШИБКА в GOTO");
            return false;
          }
          
          auto goto_action = dynamic_cast<GOTO_cell*>(goto_cell.get());
          if (!goto_action) {
            if (logs) std::cout << "ОШИБКА в GOTO\n";
            throw std::invalid_argument("ОШИБКА в GOTO");
            return false;
          }

          state_stack.push(goto_action->goto_id);
          if (logs) {
            std::cout << "Шаг " << step << ": СОКРАЩЕНИЕ по правилу " << rule.get_main() << " -> ";
            for (char c : rule.get_production()) std::cout << c;
            std::cout << ", ПЕРЕХОД в состояние " << goto_action->goto_id << "\n";
            std::cout << "        Стек состояний: [";
            print_stack(state_stack);
            std::cout << "], Вход: ";
            for (size_t i = input_index; i < input.size(); ++i) std::cout << input[i];
            std::cout << "\n";
          }
          break;
        }

        case Table_cell::Act::ACCEPT: {
          if (logs) std::cout << "Шаг " << step << ": ПРИНЯТО - Слово принадлежит грамматике!\n";
          return true;
        }
      
        case Table_cell::Act::GOTO: {
          if (logs) std::cout << "ОШИБКА: Действие GOTO в таблице разбора\n";
          return false;
        }
      
        case Table_cell::Act::ERROR: {
          if (logs) std::cout << "Шаг " << step << ": ОШИБКА - Нет действия для состояния " << current_state 
                              << " и символа '" << current_symbol << "'\n";
          return false;
        }
      
        default: {
          if (logs) std::cout << "ОШИБКА: Лажа\n";
          return false;
        }
      }
    }
  }

  void clear() {
    grammar.clear();
    action_table.reset();
    is_fitted = false;
  }

  void print_table() const {
    if (action_table) {
      action_table->print_table();
    } else {
      std::cout << "Вызови fit\n";
    }
  }

 private:
  void print_stack(std::stack<size_t> s) const {
    if (s.empty()) return;
      
    std::vector<size_t> elements;
    while (!s.empty()) {
      elements.push_back(s.top());
      s.pop();
    }
      
    for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
      std::cout << *it;
      if (it + 1 != elements.rend()) std::cout << " ";
    }
  }
};

class Wrapper {
  LR_1_parser parser;

 public:
  Wrapper() = default;

  void set_nonterminals(std::string str) {
    std::erase_if(str, [](char c)->bool {return c == ' ' || c == '\t';});
    parser.get_grammar().set_neterminals(std::vector<char>(str.begin(), str.end()));
  }

  void set_terminals(std::string str) {
    std::erase_if(str, [](char c)->bool {return c == ' ' || c == '\t';});
    parser.get_grammar().set_terminals(std::vector<char>(str.begin(), str.end()));
  }

  void add_rule(std::string str) {
    std::erase_if(str, [](char c)->bool {return c == ' ' || c == '\t';});
    auto it = str.find("->");
    if (it == std::string::npos) 
      throw std::invalid_argument("В условии сказано, что синтаксически входные данные корректны, а тут такое...");

    std::string production = str.substr(it + 2);
    if (production.empty()) {
      parser.get_grammar().add_rule(str[0], {'\x1D'});
    } else {
      parser.get_grammar().add_rule(str[0], std::vector<char>(production.begin(), production.end()));
    }
  }

  void set_start_symbol(char c) {
    parser.get_grammar().set_start_symbol(c);
  }

  void fit(bool log = false) {
    parser.get_grammar().calculate_first();
    parser.fit(log);
  }

  bool parse(const std::string& word) {
    return parser.parse(std::vector<char>(word.begin(), word.end()));;
  }

  std::vector<bool> parse_batch(const std::vector<std::string>& words) {
    std::vector<bool> result;
    for (const auto& word : words) {
      result.push_back(parse(word));
    }
    return result;
  }
};