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
class NKATest : public ::testing::Test {
protected:
    NKATest() : alphabet({'a', 'b', 'c'}), builder(alphabet) {}
    
    void SetUp() override {}
    void TearDown() override {}

    std::vector<char> alphabet;
    NKA_Builder builder;
};

TEST_F(NKATest, StateInitialization) {
    State state;
    EXPECT_FALSE(state.is_finish());
    EXPECT_EQ(state.get_links().size(), 0);
}

TEST_F(NKATest, StateAddLink) {
    State state1;
    State state2;
    
    state1.add_link('a', &state2);
    
    const auto& links = state1.get_links();
    EXPECT_EQ(links.size(), 1);
    EXPECT_EQ(links.at('a').size(), 1);
    EXPECT_EQ(links.at('a')[0], &state2);
}

TEST_F(NKATest, StateFinishFlag) {
    State state;
    state.set_finish(true);
    EXPECT_TRUE(state.is_finish());
    
    state.set_finish(false);
    EXPECT_FALSE(state.is_finish());
}

TEST_F(NKATest, TreeInitialization) {
    Tree tree;
    EXPECT_EQ(tree.buffer.size(), 0);
    EXPECT_EQ(tree.ends.size(), 0);
}

TEST_F(NKATest, TreeDestruction) {
    {
        Tree tree;
        State* s1 = new State();
        State* s2 = new State();
        tree.buffer.push_back(s1);
        tree.buffer.push_back(s2);
        tree.start = 0;
        tree.ends.push_back(1);
    }
    SUCCEED();
}

TEST_F(NKATest, BuildSingleCharacter) {
    Tree tree = builder.build("a");
    
    EXPECT_GE(tree.buffer.size(), 2);
    EXPECT_GE(tree.ends.size(), 1);

    State* start = tree.buffer[tree.start];
    State* end = tree.buffer[tree.ends[0]];
    
    EXPECT_FALSE(start->is_finish());
    EXPECT_TRUE(end->is_finish());
    
    const auto& links = start->get_links();
    EXPECT_GE(links.size(), 1);
    
    auto it = links.find('a');
    if (it != links.end()) {
        EXPECT_GE(it->second.size(), 1);
        bool leads_to_end = false;
        for (State* target : it->second) {
            if (target == end || target->is_finish()) {
                leads_to_end = true;
                break;
            }
        }
        EXPECT_TRUE(leads_to_end);
    } else {
        bool found_path = false;
        auto eps_it = links.find('\0');
        if (eps_it != links.end()) {
            for (State* intermediate : eps_it->second) {
                const auto& intermediate_links = intermediate->get_links();
                auto a_it = intermediate_links.find('a');
                if (a_it != intermediate_links.end()) {
                    found_path = true;
                    break;
                }
            }
        }
        EXPECT_TRUE(found_path);
    }
}

TEST_F(NKATest, BuildEmptyWord) {
    Tree tree = builder.build("1");
    
    EXPECT_GE(tree.buffer.size(), 2);
    EXPECT_GE(tree.ends.size(), 1);
    
    State* start = tree.buffer[tree.start];
    State* end = tree.buffer[tree.ends[0]];
    
    EXPECT_TRUE(end->is_finish());
    
    const auto& links = start->get_links();
    bool has_epsilon_to_end = false;
    auto eps_it = links.find('\0');
    if (eps_it != links.end()) {
        for (State* target : eps_it->second) {
            if (target == end || target->is_finish()) {
                has_epsilon_to_end = true;
                break;
            }
        }
    }
    EXPECT_TRUE(has_epsilon_to_end);
}

TEST_F(NKATest, BuildConcatenation) {
    Tree tree = builder.build("ab.");
    
    EXPECT_GE(tree.buffer.size(), 4);
    EXPECT_GE(tree.ends.size(), 1);
    State* start = tree.buffer[tree.start];
    EXPECT_FALSE(start->is_finish());
    
    bool has_final = false;
    for (State* state : tree.buffer) {
        if (state->is_finish()) {
            has_final = true;
            break;
        }
    }
    EXPECT_TRUE(has_final);
}

TEST_F(NKATest, BuildUnion) {
    Tree tree = builder.build("ab+");
    
    EXPECT_GE(tree.buffer.size(), 4);
    EXPECT_GE(tree.ends.size(), 1);
}

TEST_F(NKATest, BuildStar) {
    Tree tree = builder.build("a*");
    
    EXPECT_GE(tree.buffer.size(), 3);
    EXPECT_GE(tree.ends.size(), 1);

    State* start = tree.buffer[tree.start];
    bool has_epsilon_to_final = false;
    
    const auto& links = start->get_links();
    auto eps_it = links.find('\0');
    if (eps_it != links.end()) {
        for (State* target : eps_it->second) {
            if (target->is_finish()) {
                has_epsilon_to_final = true;
                break;
            }
        }
    }
    EXPECT_TRUE(has_epsilon_to_final);
}

TEST_F(NKATest, BuildComplexExpression) {
    Tree tree = builder.build("ab.*c+");
    
    EXPECT_GT(tree.buffer.size(), 0);
    EXPECT_GT(tree.ends.size(), 0);
}

TEST_F(NKATest, BuildThrowsOnInsufficientOperandsForPlus) {
    EXPECT_THROW(builder.build("+"), std::exception);
}

TEST_F(NKATest, BuildThrowsOnInsufficientOperandsForConcatenation) {
    EXPECT_THROW(builder.build("."), std::exception);
}

TEST_F(NKATest, BuildThrowsOnInsufficientOperandsForStar) {
    EXPECT_THROW(builder.build("*"), std::exception);
}

TEST_F(NKATest, StarOperationStructure) {
    Tree tree = builder.build("a*");
    
    State* start = tree.buffer[tree.start];
    const auto& links = start->get_links();

    auto eps_it = links.find('\0');
    EXPECT_TRUE(eps_it != links.end());
    EXPECT_GE(eps_it->second.size(), 1);
}

TEST_F(NKATest, UnionOperationStructure) {
    Tree tree = builder.build("ab+");
    
    State* start = tree.buffer[tree.start];
    const auto& links = start->get_links();

    auto eps_it = links.find('\0');
    EXPECT_TRUE(eps_it != links.end());
    EXPECT_GE(eps_it->second.size(), 2);
}

TEST_F(NKATest, TreeMoveConstructor) {
    Tree original = builder.build("a");
    size_t originalSize = original.buffer.size();
    size_t originalEnds = original.ends.size();
    size_t originalStart = original.start;
    
    Tree moved(std::move(original));
    
    EXPECT_EQ(moved.buffer.size(), originalSize);
    EXPECT_EQ(moved.ends.size(), originalEnds);
    EXPECT_EQ(moved.start, originalStart);

    EXPECT_EQ(original.buffer.size(), 0);
    EXPECT_EQ(original.ends.size(), 0);
}

TEST_F(NKATest, DifferentAlphabets) {
    std::vector<char> custom_alphabet = {'x', 'y', 'z'};
    NKA_Builder custom_builder(custom_alphabet);
    
    Tree tree = custom_builder.build("xy.z+");
    
    EXPECT_GT(tree.buffer.size(), 0);
    EXPECT_GT(tree.ends.size(), 0);
}

TEST_F(NKATest, PrintTreeDoesNotCrash) {
    Tree tree = builder.build("ab+*");

    EXPECT_NO_THROW(tree.print_Tree());
}

TEST_F(NKATest, MultipleOperations) {
    Tree tree = builder.build("a*b.c+*");
    
    EXPECT_GT(tree.buffer.size(), 0);
    EXPECT_GT(tree.ends.size(), 0);
    
    bool hasFinalState = false;
    for (State* state : tree.buffer) {
        if (state->is_finish()) {
            hasFinalState = true;
            break;
        }
    }
    EXPECT_TRUE(hasFinalState);
}