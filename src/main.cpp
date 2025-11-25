#include "Getter_Grammatic.h"

int main() {
  int n;
  int sigma_size;
  size_t p;
  std::cin >> n >> sigma_size >> p;
  
  std::string neterminals_str;
  std::cin >> neterminals_str;
  
  std::string alphabet_str;
  std::cin >> alphabet_str;
  
  Erly_Machine machine;
  machine.Set_Alphabet(alphabet_str);
  machine.Set_Neterminals(neterminals_str);

  std::cin.ignore();
  
  std::string rule;
  for (size_t i = 0; i < p; ++i) {
    std::getline(std::cin, rule);
    machine.Add_Link(rule);
  }

  std::string start_symbol;
  std::cin >> start_symbol;
  machine.Set_Start_Symbol(start_symbol[0]);
  
  int m;
  std::cin >> m;
  
  for (int i = 0; i < m; ++i) {
    std::string word;
    std::cin >> word;
    
    try {
      machine.Get_word(word);
      if (machine.is_word_in_grammar()) {
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