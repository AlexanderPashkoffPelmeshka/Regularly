
#include "Machine.h"
#include <gtest/gtest.h>

TEST(KilledOptimisationTest, SetsKillFlagForClosedState) {
    DFA dfa(0);

    DFA_State* state0 = dfa.add_state(0);
    DFA_State* state1 = dfa.add_state(1);

    state0->add_link('a', 0);
    state0->add_link('b', 0);

    state1->add_link('a', 0);
    state1->add_link('b', 1);

    EXPECT_TRUE(state0->is_closed());

    EXPECT_FALSE(state1->is_closed());

    killed_optimisation(dfa);

    EXPECT_TRUE(state0->get_kill_optimisation());

    EXPECT_FALSE(state1->get_kill_optimisation());
}

TEST(KilledOptimisationTest, NoClosedStateNoKillFlag) {
    DFA dfa(0);

    DFA_State* state0 = dfa.add_state(0);
    DFA_State* state1 = dfa.add_state(1);

    state0->add_link('a', 1);
    state0->add_link('b', 0);
    state1->add_link('a', 0);
    state1->add_link('b', 1);

    EXPECT_FALSE(state0->is_closed());
    EXPECT_FALSE(state1->is_closed());

    killed_optimisation(dfa);

    EXPECT_FALSE(state0->get_kill_optimisation());
    EXPECT_FALSE(state1->get_kill_optimisation());
}

TEST(KilledOptimisationTest, SingleStateDFA) {
    DFA dfa(0);
    DFA_State* state0 = dfa.add_state(0);

    state0->add_link('a', 0);
    state0->add_link('b', 0);
    
    EXPECT_TRUE(state0->is_closed());

    killed_optimisation(dfa);

    EXPECT_TRUE(state0->get_kill_optimisation());
}

TEST(KilledOptimisationTest, MultipleClosedStatesSetsFirstOnly) {
    DFA dfa(0);
    
    DFA_State* state0 = dfa.add_state(0);
    DFA_State* state1 = dfa.add_state(1);
    DFA_State* state2 = dfa.add_state(2);

    state0->add_link('a', 0);
    state0->add_link('b', 0);
    
    state1->add_link('a', 1);
    state1->add_link('b', 1);
    
    state2->add_link('a', 2);
    state2->add_link('b', 2);

    EXPECT_TRUE(state0->is_closed());
    EXPECT_TRUE(state1->is_closed());
    EXPECT_TRUE(state2->is_closed());

    killed_optimisation(dfa);

    EXPECT_TRUE(state0->get_kill_optimisation());
    EXPECT_FALSE(state1->get_kill_optimisation());
    EXPECT_FALSE(state2->get_kill_optimisation());
}