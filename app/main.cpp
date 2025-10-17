#include <Machine.h>

int main() {
  Machine machine({'a', 'b', 'c'});
  std::string word;
  std::string polish_language;

  std::cout << "Введите язык в обратной польской записи: ";
  std::cin >> polish_language;
  std::cout << "Введите слово, максимальный префикс которого я найду: ";
  std::cin >> word;

  DFA mdfa = machine.Build_mpdka(polish_language);
  std::cout << mdfa.Task_15(word);
  return 0;
}