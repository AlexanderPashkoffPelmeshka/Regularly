#pragma once
#include <algorithm>
#include <cstdint>
#include <format>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "Parser.h"
#include "NKA_parser.h"
#include "DKA.h"
#include "PDKA_parser.h"
#include "Optimisations.h"
#include "MPDKA_parser.h"

template <typename NKA_builder = NKA_Builder, typename PDKA_builder = PDKA_Builder,
typename MPDKA_builder = MPDKA_Builder, Parser_Reg Parser = Parser_Ordinary>
class Machine {
  NKA_Builder nka_builder;
  PDKA_builder pdka_builder;
  MPDKA_builder mpdka_builder;
  Parser parser;

 public:
  Machine(const std::vector<char>& alphabets) : nka_builder(alphabets),
  pdka_builder(alphabets), mpdka_builder(alphabets) {};
  
  Machine(const std::vector<char>& alphabets_first,
          const std::vector<char>& alphabets_second,
          const std::vector<char>& alphabets_third) : 
          nka_builder(alphabets_first),
          pdka_builder(alphabets_second),
          mpdka_builder(alphabets_third) {};

  Tree Build_nka(const std::string& str) {
    std::string polish = parser.parse_regularly(str);
    return nka_builder.build(polish);
  }

  DFA Build_pdka(const std::string& str) {
    std::string polish = parser.parse_regularly(str);
    return pdka_builder.build(nka_builder.build(polish));
  }

  DFA Build_mpdka(const std::string& str) {
    std::string polish = parser.parse_regularly(str);
    return mpdka_builder.build(pdka_builder.build(nka_builder.build(polish)));
  }
};