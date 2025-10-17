#pragma once

template <typename T>
concept Parser_Reg = requires(T a) {
  { a.parse_regularly("a") } -> std::same_as<std::string>;
};

class Parser_Ordinary {
 public:
  static std::string parse_regularly(const std::string& str) {
    return str;
  }
};