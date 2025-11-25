#include "LR.h"
#include <cassert>
#include <iostream>

void Test1() {
  std::cout << "=== Test 1: Simple grammar S->a ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("a");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->a");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"a", "", "aa", "b"};
    bool expected[] = {1, 0, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "1." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 1 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 1 ERROR: " << e.what() << "\n\n";
  }
}

void Test2() {
  std::cout << "=== Test 2: Grammar S->ab ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("ab");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->ab");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"ab", "a", "b", "aa", "abb"};
    bool expected[] = {1, 0, 0, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "2." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 2 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 2 ERROR: " << e.what() << "\n\n";
  }
}

void Test3() {
  std::cout << "=== Test 3: Grammar S->a|b ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("ab");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->a");
    wrapper.add_rule("S->b");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"a", "b", "c", "ab"};
    bool expected[] = {1, 1, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "3." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 3 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 3 ERROR: " << e.what() << "\n\n";
  }
}

void Test4() {
  std::cout << "=== Test 4: Grammar with epsilon rules ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("SA");
    wrapper.set_terminals("a");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->AS");
    wrapper.add_rule("S->");
    wrapper.add_rule("A->a");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"", "a", "aa", "aaa", "b"};
    bool expected[] = {1, 1, 1, 1, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "4." << i + 1 << ": ";
      if (test_cases[i].empty()) std::cout << "ε";
      else std::cout << "\"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 4 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 4 ERROR: " << e.what() << "\n\n";
  }
}

void Test5() {
  std::cout << "=== Test 5: Arithmetic expressions ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("ETF");
    wrapper.set_terminals("+*()i");
    wrapper.set_start_symbol('E');
    wrapper.add_rule("E->E+T");
    wrapper.add_rule("E->T");
    wrapper.add_rule("T->T*F");
    wrapper.add_rule("T->F");
    wrapper.add_rule("F->(E)");
    wrapper.add_rule("F->i");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"i", "i+i", "i*i", "i+i*i", "(i+i)*i", "((i))", "i+", "+i", "(i"};
    bool expected[] = {1, 1, 1, 1, 1, 1, 0, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "5." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 5 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 5 ERROR: " << e.what() << "\n\n";
  }
}

void Test6() {
  std::cout << "=== Test 6: Left recursion S->Sa|a ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("a");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->Sa");
    wrapper.add_rule("S->a");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"a", "aa", "aaa", "aaaa", "", "b"};
    bool expected[] = {1, 1, 1, 1, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "6." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 6 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 6 ERROR: " << e.what() << "\n\n";
  }
}

void Test7() {
  std::cout << "=== Test 7: Right recursion S->aS|a ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("a");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->aS");
    wrapper.add_rule("S->a");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"a", "aa", "aaa", ""};
    bool expected[] = {1, 1, 1, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "7." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 7 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 7 ERROR: " << e.what() << "\n\n";
  }
}

void Test8() {
  std::cout << "=== Test 8: Complex grammar with statements ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("PSEF");
    wrapper.set_terminals("i=+*;{}");
    wrapper.set_start_symbol('P');
    wrapper.add_rule("P->S;P");
    wrapper.add_rule("P->S");
    wrapper.add_rule("S->i=E");
    wrapper.add_rule("S->{P}");
    wrapper.add_rule("E->E+T");
    wrapper.add_rule("E->T");
    wrapper.add_rule("T->T*F");
    wrapper.add_rule("T->F");
    wrapper.add_rule("F->i");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"i=i", "i=i+i", "{i=i}", "i=i;i=i", "{i=i;i=i}", "i=i+", "{i=i"};
    bool expected[] = {1, 1, 1, 1, 1, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "8." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 8 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 8 ERROR: " << e.what() << "\n\n";
  }
}

void Test9() {
  std::cout << "=== Test 9: Spaces in grammar definition ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S  A  B");
    wrapper.set_terminals("a  b");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S -> A B");
    wrapper.add_rule("A-> a");
    wrapper.add_rule("B ->b");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"ab", "a"};
    bool expected[] = {1, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "9." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 9 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 9 ERROR: " << e.what() << "\n\n";
  }
}

void Test10() {
  std::cout << "=== Test 10: Batch parsing ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("SAB");
    wrapper.set_terminals("ab");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->AB");
    wrapper.add_rule("A->a");
    wrapper.add_rule("B->b");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"ab", "a", "b", "aa", "bb", "ba"};
    bool expected[] = {1, 0, 0, 0, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "10." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 10 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 10 ERROR: " << e.what() << "\n\n";
  }
}

void Test11() {
  std::cout << "=== Test 11: Multiple nonterminals ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("SABCD");
    wrapper.set_terminals("abc");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->ABC");
    wrapper.add_rule("A->a");
    wrapper.add_rule("B->b");
    wrapper.add_rule("C->c");
    wrapper.add_rule("D->d");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"abc", "ab", "abcd"};
    bool expected[] = {1, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "11." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 11 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 11 ERROR: " << e.what() << "\n\n";
  }
}

void Test12() {
  std::cout << "=== Test 12: Empty string grammar ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("a");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"", "a"};
    bool expected[] = {1, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "12." << i + 1 << ": ";
      if (test_cases[i].empty()) std::cout << "ε";
      else std::cout << "\"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 12 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 12 ERROR: " << e.what() << "\n\n";
  }
}

void Test13() {
  std::cout << "=== Test 13: Operator priority ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("ETF");
    wrapper.set_terminals("+*()i");
    wrapper.set_start_symbol('E');
    wrapper.add_rule("E->E+T");
    wrapper.add_rule("E->T");
    wrapper.add_rule("T->T*F");
    wrapper.add_rule("T->F");
    wrapper.add_rule("F->(E)");
    wrapper.add_rule("F->i");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"i+i*i", "(i+i)*i", "i*i+i"};
    bool expected[] = {1, 1, 1};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "13." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 13 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 13 ERROR: " << e.what() << "\n\n";
  }
}

void Test14() {
  std::cout << "=== Test 14: Long chains ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("a");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->aS");
    wrapper.add_rule("S->a");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"aaaaaaaaaa", "aaaaa"};
    bool expected[] = {1, 1};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "14." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 14 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 14 ERROR: " << e.what() << "\n\n";
  }
}

void Test15() {
  std::cout << "=== Test 15: Complex expressions ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("ETF");
    wrapper.set_terminals("+*()i");
    wrapper.set_start_symbol('E');
    wrapper.add_rule("E->E+T");
    wrapper.add_rule("E->T");
    wrapper.add_rule("T->T*F");
    wrapper.add_rule("T->F");
    wrapper.add_rule("F->(E)");
    wrapper.add_rule("F->i");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"i+(i*i)", "(i+i)*(i+i)", "((i+i)*i)+i", "i*(i+(i*i))"};
    bool expected[] = {1, 1, 1, 1};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "15." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 15 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 15 ERROR: " << e.what() << "\n\n";
  }
}

void Test16() {
  std::cout << "=== Test 16: Mixed rules ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("SAB");
    wrapper.set_terminals("abc");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->A");
    wrapper.add_rule("S->B");
    wrapper.add_rule("A->a");
    wrapper.add_rule("A->aA");
    wrapper.add_rule("B->b");
    wrapper.add_rule("B->bB");
    wrapper.add_rule("B->c");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"a", "aa", "aaa", "b", "bb", "c", "bc"};
    bool expected[] = {1, 1, 1, 1, 1, 1, 1};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "16." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 16 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 16 ERROR: " << e.what() << "\n\n";
  }
}

void Test17() {
  std::cout << "=== Test 17: Error recovery ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("S");
    wrapper.set_terminals("ab");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->ab");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {"abc", "a ", "ba"};
    bool expected[] = {0, 0, 0};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "17." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 17 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 17 ERROR: " << e.what() << "\n\n";
  }
}

void Test18() {
  std::cout << "=== Test 18: Comprehensive grammar ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("SEF");
    wrapper.set_terminals("abcd+*()");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->S+E");
    wrapper.add_rule("S->E");
    wrapper.add_rule("E->E*F");
    wrapper.add_rule("E->F");
    wrapper.add_rule("F->(S)");
    wrapper.add_rule("F->a");
    wrapper.add_rule("F->b");
    wrapper.add_rule("F->c");
    wrapper.add_rule("F->d");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {
      "a", "a+b", "a*b", "a+b*c", "(a+b)*c", "a+(b*c)", 
      "((a))", "a+", "+a", "a+b+", "a*b*c*d"
    };
    bool expected[] = {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "18." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 18 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 18 ERROR: " << e.what() << "\n\n";
  }
}

void Test19() {
  std::cout << "=== Test 19: Ambiguous grammar ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("SIE");
    wrapper.set_terminals("ia{}");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->I");
    wrapper.add_rule("S->S");
    wrapper.add_rule("I->iE");
    wrapper.add_rule("I->iES");
    wrapper.add_rule("I->iESI");
    wrapper.add_rule("E->a");
    wrapper.add_rule("S->{S}");
    wrapper.add_rule("S->");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {
      "ia", "iaia", "i{a}", "i{ia}", "i{i{a}}", 
      "iai{a}", "i{a}i{a}", "{ia}", "{{ia}}"
    };
    bool expected[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "19." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 19 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 19 ERROR: " << e.what() << " - " << "Ошибка в reduce" << "\n\n";
  }
}

void Test20() {
  std::cout << "=== Test 20: Deep recursion ===\n";
  try {
    Wrapper wrapper;
    wrapper.set_nonterminals("SAB");
    wrapper.set_terminals("abc()");
    wrapper.set_start_symbol('S');
    wrapper.add_rule("S->A");
    wrapper.add_rule("S->B");
    wrapper.add_rule("A->a");
    wrapper.add_rule("A->(A)");
    wrapper.add_rule("A->aA");
    wrapper.add_rule("A->(S)");
    wrapper.add_rule("B->b");
    wrapper.add_rule("B->(B)");
    wrapper.add_rule("B->bB");
    wrapper.add_rule("B->b(S)");
    wrapper.add_rule("B->(S)B");
    wrapper.fit(false);
    
    std::vector<std::string> test_cases = {
      "a", "((a))", "(((a)))", "a(a)", "a((a))", 
      "b", "(b)", "((b))", "b(b)", "b((b))",
      "a(b)", "(a)b", "((a)(b))", "a(b(a))", "((a)b(a))"
    };
    bool expected[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    for (size_t i = 0; i < test_cases.size(); ++i) {
      std::cout << "20." << i + 1 << ": \"" << test_cases[i] << "\"";
      try {
        bool result = wrapper.parse(test_cases[i]);
        std::cout << " -> result: " << result;
        assert(result == expected[i]);
        std::cout << " [OK]\n";
      } catch (...) {
        std::cout << " -> result: get_error";
        assert(0 == expected[i]);
        std::cout << " [OK]\n";
      }
    }
    std::cout << "Test 20 PASSED\n\n";
  } catch (const std::exception& e) {
    std::cout << "Test 20 ERROR: " << e.what() << " - " << "Конфликт для символа" << "\n\n";
  }
}

int main() {
  Test1();
  Test2();
  Test3();
  Test4();
  Test5();
  Test6();
  Test7();
  Test8();
  Test9();
  Test10();
  Test11();
  Test12();
  Test13();
  Test14();
  Test15();
  Test16();
  Test17();
  Test18();
  Test19();
  Test20();
  
  std::cout << "=== ALL TESTS COMPLETED ===\n";
  return 0;
}