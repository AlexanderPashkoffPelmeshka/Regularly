#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <string_view>
#include <memory>
#include <numeric>

class Getter_Grammatic {
  std::unordered_map<char, uint16_t> hesher;
  std::unordered_map<uint16_t, char> reverse_hesher;
  std::vector<char> alphabet;
  std::vector<char> neterminals;
  std::unordered_map<uint16_t, std::vector<std::vector<int>>> links;
  const int16_t offset = 1024;

public:
  Getter_Grammatic() {
    hesher.insert({'\x1D', 0});
    reverse_hesher.insert({0, '\x1D'});
    alphabet.push_back('\x1D');
  }

  bool Set_Alphabet(const std::string& str) {
    if (str.size() == 0) throw std::invalid_argument("Zero alphabet is bad");
    alphabet.clear();
    alphabet.push_back('\x1D');
    if (str.empty()) {
      return true;
    }
    for (size_t i = 0; i < str.size(); ++i) {
      if (hesher.find(str[i]) != hesher.end()) {
        throw std::invalid_argument("Duplicate symbol in alphabet");
      }
      hesher[str[i]] = i + 1;
      reverse_hesher[i + 1] = str[i];
      alphabet.push_back(str[i]);
    }
    return true;
  }

  bool Set_Neterminals(const std::string& str) {
    neterminals.clear();
    for (size_t i = 0; i < str.size(); ++i) {
      if (hesher.find(str[i]) != hesher.end()) {
        throw std::invalid_argument("Duplicate symbol in neterminal");
      }
      hesher[str[i]] = i + offset;
      reverse_hesher[i + offset] = str[i];
      neterminals.push_back(str[i]);
    }
    return true;
  }

  void Add_Link(std::string str) {
    std::erase_if(str, [](char c)->bool {return c == ' ' || c == '\t';});
    // std::cout << str << "\n";
    size_t arrow_pos = str.find("->");
    // std::cout << arrow_pos << "\n";
    if (arrow_pos != std::string_view::npos) {
      std::string_view before = std::string_view(str).substr(0, arrow_pos);
      std::string_view after = std::string_view(str).substr(arrow_pos + 2);

      if (before.empty()) {
        std::cout << before << "\n";
        throw std::invalid_argument("Empty left part in rule");
      }

      if (before.size() > 1) {
        throw std::invalid_argument("Left part must be single character");
      } 
      
      if (hesher.find(before[0]) == hesher.end()) {
        throw std::invalid_argument("Unknown nonterminal in left part");
      }

      uint16_t left_hash = hesher[before[0]];

      if (!is_nonterminal(left_hash)) {
        throw std::invalid_argument("Left part must be nonterminal");
      }

      std::vector<int> right_part;
      for (char c : after) {
        if (hesher.find(c) == hesher.end()) {
          throw std::invalid_argument(std::string("Unknown symbol: ") + c);
        }

        right_part.push_back(hesher[c]);
      }

      if (right_part.size() == 0) right_part.push_back(0);

      links[left_hash].push_back(right_part);
      
    } else {
      throw std::invalid_argument("Invalid Link: no '->' found");
    }
  }

  uint16_t get_hash(const char c) const {
    auto it = hesher.find(c);
    if (it == hesher.end()) throw std::invalid_argument("Character not found in grammar");
    return it->second;
  }

  char get_char_by_hash(uint16_t hash) const {
    auto it = reverse_hesher.find(hash);
    if (it == reverse_hesher.end()) throw std::invalid_argument("Hash not found in grammar");
    return it->second;
  }

  const std::unordered_map<uint16_t, std::vector<std::vector<int>>>& get_links() const { return links; }
  const std::vector<char>& get_neterminals() const { return neterminals; }
  const std::vector<char>& get_alphabet() const { return alphabet; }
  bool is_terminal(uint16_t hash) const { return hash < offset && hash != 0; }
  bool is_nonterminal(uint16_t hash) const { return hash >= offset; }
  bool is_epsilon(uint16_t hash) const { return hash == 0; }
};

struct Situation {
  uint16_t left;
  std::vector<int> right;
  size_t dot_pos;
  size_t origin;

  Situation(uint16_t l, const std::vector<int>& r, size_t dot, size_t orig)
    : left(l), right(r), dot_pos(dot), origin(orig) {}

  bool operator==(const Situation& other) const {
    return left == other.left && right == other.right && 
           dot_pos == other.dot_pos && origin == other.origin;
  }

  bool operator<(const Situation& other) const {
    if (left != other.left) return left < other.left;
    if (right != other.right) return right < other.right;
    if (dot_pos != other.dot_pos) return dot_pos < other.dot_pos;
    return origin < other.origin;
  }
};

struct SituationHash {
  size_t operator()(const Situation& s) const {
    size_t h = std::hash<uint16_t>()(s.left);
    h = h * 31 + std::hash<size_t>()(s.dot_pos);
    h = h * 31 + std::hash<size_t>()(s.origin);
    for (int sym : s.right) {
      h = h * 31 + std::hash<int>()(sym);
    }
    return h;
  }
};

class Erly_Machine {
private:
  Getter_Grammatic grammar;
  std::vector<std::unordered_set<Situation, SituationHash>> sets;
  std::string processed_word;
  bool word_in_grammar;
  uint16_t start_symbol;
  bool start_symbol_defined;

  void predict(size_t set_index, const Situation& situation) {
    uint16_t next_symbol = situation.right[situation.dot_pos];
    
    for (const auto& rule : grammar.get_links().at(next_symbol)) {
      Situation new_situation(next_symbol, rule, 0, set_index);
      if (sets[set_index].insert(new_situation).second) {
        if (!rule.empty() && grammar.is_nonterminal(rule[0])) {
          predict(set_index, new_situation);
        }
      }
    }
  }

  void scan(size_t set_index, const Situation& situation, char next_char) {
    if (set_index >= processed_word.length()) return;
    
    uint16_t next_symbol = situation.right[situation.dot_pos];
    uint16_t char_hash = grammar.get_hash(next_char);
    
    if (next_symbol == char_hash) {
      Situation new_situation(situation.left, situation.right,situation.dot_pos + 1, situation.origin);
      sets[set_index + 1].insert(new_situation);
    }
  }

  void complete(size_t set_index, const Situation& situation) {
    size_t origin_set = situation.origin;
    
    auto situations_to_check = sets[origin_set];
    
    for (const Situation& orig_situation : situations_to_check) {
      if (orig_situation.dot_pos < orig_situation.right.size() &&
          orig_situation.right[orig_situation.dot_pos] == situation.left) {
        
        Situation new_situation(orig_situation.left, orig_situation.right,
                              orig_situation.dot_pos + 1, orig_situation.origin);
        
        if (sets[set_index].insert(new_situation).second) {
          if (new_situation.dot_pos < new_situation.right.size() &&
              grammar.is_nonterminal(new_situation.right[new_situation.dot_pos])) {
            predict(set_index, new_situation);
          }
        }
      }
    }
  }

public:
  Erly_Machine() : word_in_grammar(false), start_symbol_defined(false) {}

  void Set_Alphabet(const std::string& str) {
    grammar.Set_Alphabet(str);
  }

  void Set_Neterminals(const std::string& str) {
    grammar.Set_Neterminals(str);
  }

  void Add_Link(const std::string& str) {
    grammar.Add_Link(str);
  }

  void Set_Start_Symbol(char c) {
    start_symbol = grammar.get_hash(c);
    if (!grammar.is_nonterminal(start_symbol)) {
      throw std::invalid_argument("Start symbol must be nonterminal");
    }
    start_symbol_defined = true;
  }
  
  void Get_word(const std::string& word) {
    if (grammar.get_links().empty()) throw std::invalid_argument("You has not rules!!!");

    processed_word = word;
    word_in_grammar = false;

    if (!start_symbol_defined) {
      if (grammar.get_neterminals().empty()) {
        throw std::invalid_argument("No start symbol defined in grammar");
      }
      start_symbol = grammar.get_hash(grammar.get_neterminals()[0]);
      start_symbol_defined = true;
    }

    for (char c : word) {
      try {
        grammar.get_hash(c);
      } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Word contains character '" + 
                                  std::string(1, c) + "' not in alphabet");
      }
    }

    sets.clear();
    sets.resize(word.length() + 1);

    auto start_rules_it = grammar.get_links().find(start_symbol);
    if (start_rules_it == grammar.get_links().end()) {
      throw std::invalid_argument("No rules defined for start symbol '" + 
                                std::string(1, grammar.get_char_by_hash(start_symbol)) + "'");
    }

    for (const auto& rule : start_rules_it->second) {
      Situation start_situation(start_symbol, rule, 0, 0);
      sets[0].insert(start_situation);
    }

    for (size_t i = 0; i <= word.length(); ++i) {
      bool changed;
      size_t iteration_count = 0;
      const size_t max_iterations = 10000;
      
      do {
        changed = false;
        iteration_count++;
        
        if (iteration_count > max_iterations) {
          throw std::invalid_argument("A lot of iteration/ maybe error (but mustnt)");
        }
        
        auto current_set = sets[i];
        
        for (const Situation& situation : current_set) {
          if (situation.dot_pos >= situation.right.size()) {
            complete(i, situation);
            continue;
          }
          
          uint16_t next_symbol = situation.right[situation.dot_pos];
          
          if (grammar.is_terminal(next_symbol)) {
            if (i < word.length()) {
              scan(i, situation, word[i]);
            }
          }
          else if (grammar.is_nonterminal(next_symbol)) {
            predict(i, situation);
            
            auto new_rules_it = grammar.get_links().find(next_symbol);
            if (new_rules_it != grammar.get_links().end()) {
              for (const auto& new_rule : new_rules_it->second) {
                if (new_rule.empty() || (new_rule.size() == 1 && grammar.is_epsilon(new_rule[0]))) {
                  Situation epsilon_situation(next_symbol, new_rule, new_rule.size(), i);
                  if (sets[i].insert(epsilon_situation).second) {
                    complete(i, epsilon_situation);
                  }
                }
              }
            }
          }
        }
        
        if (sets[i] != current_set) {
          changed = true;
        }
      } while (changed);
    }
    
    for (const Situation& situation : sets[word.length()]) {
      if (situation.left == start_symbol &&
          situation.dot_pos == situation.right.size() &&
          situation.origin == 0) {
        word_in_grammar = true;
        break;
      }
    }
  }

  bool is_word_in_grammar() const { return word_in_grammar; }
};