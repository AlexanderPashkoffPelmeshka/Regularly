#include "LR.h"

int main() {
  int n;
  int sigma_size;
  size_t p;
  std::cin >> n >> sigma_size >> p;
  
  std::string neterminals_str;
  std::cin >> neterminals_str;
  
  std::string alphabet_str;
  std::cin >> alphabet_str;
  
  Wrapper machine;
  machine.set_terminals(alphabet_str);
  machine.set_nonterminals(neterminals_str);

  std::cin.ignore();
  
  std::string rule;
  for (size_t i = 0; i < p; ++i) {
    std::getline(std::cin, rule);
    machine.add_rule(rule);
  }

  std::string start_symbol;
  std::cin >> start_symbol;
  machine.set_start_symbol(start_symbol[0]);
  
  machine.fit();

  int m;
  std::cin >> m;
  
  for (int i = 0; i < m; ++i) {
    std::string word;
    std::cin >> word;
    
    try {
      if (machine.parse(word)) {
        std::cout << "Yes\n";
      } else {
        std::cout << "No\n";
      }
    } catch (const std::exception& e) {
      std::cout << "No\n";
    }
  }
  
  return 0;
}