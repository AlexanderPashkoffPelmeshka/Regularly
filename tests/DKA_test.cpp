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

TEST(DFAStateTest, Initialization) {
    DFA_State state(0);
    EXPECT_EQ(state.get_id(), 0);
    EXPECT_FALSE(state.is_finish());
    EXPECT_FALSE(state.get_kill_optimisation());
}

TEST(DFAStateTest, AddAndGetLinks) {
    DFA_State state(0);
    state.add_link('a', 1);
    state.add_link('b', 2);
    
    auto neighbor_a = state.get_neighbour('a');
    auto neighbor_b = state.get_neighbour('b');
    auto neighbor_c = state.get_neighbour('c');
    
    EXPECT_TRUE(neighbor_a.has_value());
    EXPECT_EQ(neighbor_a.value(), 1);
    EXPECT_TRUE(neighbor_b.has_value());
    EXPECT_EQ(neighbor_b.value(), 2);
    EXPECT_FALSE(neighbor_c.has_value());
}

TEST(DFAStateTest, FinishState) {
    DFA_State state(0);
    state.set_finish(true);
    EXPECT_TRUE(state.is_finish());
    state.set_finish(false);
    EXPECT_FALSE(state.is_finish());
}

TEST(DFAStateTest, KillOptimisation) {
    DFA_State state(0);
    state.set_kill_optimisation(true);
    EXPECT_TRUE(state.get_kill_optimisation());
}

TEST(DFAStateTest, IsClosed) {
    DFA_State state(0);
    state.add_link('a', 0);
    state.add_link('b', 0);
    EXPECT_TRUE(state.is_closed());
    
    state.add_link('c', 1);
    EXPECT_FALSE(state.is_closed());
}

TEST(DFATest, Initialization) {
    DFA dfa(0);
    EXPECT_EQ(dfa.get_start_state(), 0);
    EXPECT_EQ(dfa.get_states_count(), 0);
}

TEST(DFATest, AddAndGetStates) {
    DFA dfa(0);
    DFA_State* state0 = dfa.add_state(0);
    DFA_State* state1 = dfa.add_state(1);
    
    EXPECT_EQ(dfa.get_states_count(), 2);
    EXPECT_EQ(dfa.get_state(0), state0);
    EXPECT_EQ(dfa.get_state(1), state1);
    EXPECT_EQ(dfa.get_state(2), nullptr);
}

TEST(DFATest, FinalStates) {
    DFA dfa(0);
    dfa.add_final_state(1);
    dfa.add_final_state(2);
    
    const auto& finals = dfa.get_final_states();
    EXPECT_EQ(finals.size(), 2);
    EXPECT_EQ(finals[0], 1);
    EXPECT_EQ(finals[1], 2);
}

TEST(DFATest, MoveConstructor) {
    DFA original(0);
    original.add_state(0);
    original.add_final_state(0);
    
    DFA moved(std::move(original));
    EXPECT_EQ(moved.get_start_state(), 0);
    EXPECT_EQ(moved.get_states_count(), 1);
    EXPECT_EQ(moved.get_final_states().size(), 1);
}

TEST(DFATest, Task15BasicMatching) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    DFA_State* s1 = dfa.add_state(1);
    s1->set_finish(true);
    
    s0->add_link('a', 1);
    s1->add_link('a', 1);
    
    EXPECT_EQ(dfa.Task_15("a"), 1);
    EXPECT_EQ(dfa.Task_15("aaa"), 3);
    EXPECT_EQ(dfa.Task_15("b"), 0);
}

TEST(DFATest, Task15KillOptimisation) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    DFA_State* s1 = dfa.add_state(1);
    s1->set_kill_optimisation(true);
    
    s0->add_link('a', 1);
    s1->add_link('a', 1);
    
    EXPECT_EQ(dfa.Task_15("aaa"), 0);
}

TEST(DFATest, Task15PartialMatch) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    DFA_State* s1 = dfa.add_state(1);
    DFA_State* s2 = dfa.add_state(2);
    s1->set_finish(true);
    
    s0->add_link('a', 1);
    s1->add_link('b', 2);
    s2->add_link('c', 2);
    
    EXPECT_EQ(dfa.Task_15("abc"), 1);
}