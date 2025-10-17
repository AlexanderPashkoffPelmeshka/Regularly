#include <gtest/gtest.h>
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

#include <gtest/gtest.h>

TEST(ParserConceptTest, OrdinaryParserSatisfiesConcept) {
    static_assert(Parser_Reg<Parser_Ordinary>, "Parser_Ordinary должен удовлетворять Parser_Reg");
    EXPECT_TRUE((Parser_Reg<Parser_Ordinary>));
}

TEST(ParserConceptTest, OrdinaryParserHasCorrectMethod) {
    EXPECT_TRUE(requires(Parser_Ordinary a) {
        { a.parse_regularly("a") } -> std::same_as<std::string>;
    });
}

TEST(ParserOrdinaryTest, ReturnsSameString) {
    std::string input = "test_string";
    std::string result = Parser_Ordinary::parse_regularly(input);
    EXPECT_EQ(result, input);
}

TEST(ParserOrdinaryTest, HandlesEmptyString) {
    std::string input = "";
    std::string result = Parser_Ordinary::parse_regularly(input);
    EXPECT_EQ(result, input);
}

TEST(ParserOrdinaryTest, HandlesSpecialCharacters) {
    std::string input = "!@#$%^&*()";
    std::string result = Parser_Ordinary::parse_regularly(input);
    EXPECT_EQ(result, input);
}

class BadParser {
public:
    int parse_regularly(const std::string&) { return 42; }
};

TEST(ParserConceptTest, BadParserFailsConcept) {
    static_assert(!Parser_Reg<BadParser>, "BadParser не должен удовлетворять Parser_Reg");
    EXPECT_FALSE((Parser_Reg<BadParser>));
}