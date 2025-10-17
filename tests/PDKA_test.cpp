#include <gtest/gtest.h>
#include "Machine.h"

class PDKATest : public ::testing::Test {
protected:
    PDKATest() : alphabet({'a', 'b', 'c'}), nka_builder(alphabet), pdka_builder(alphabet) {}
    
    void SetUp() override {
    }

    void TearDown() override {
    }

    std::vector<char> alphabet;
    NKA_Builder nka_builder;
    PDKA_Builder pdka_builder;
};

TEST_F(PDKATest, BuildSimpleNFAtoDFA) {
    Tree nka = nka_builder.build("a");
    DFA dfa = pdka_builder.build(nka);
    
    EXPECT_GE(dfa.get_states_count(), 1);
    EXPECT_EQ(dfa.get_start_state(), 0);
    EXPECT_GT(dfa.get_final_states().size(), 0);
}

TEST_F(PDKATest, BuildDFAFromSingleCharacter) {
    Tree nka = nka_builder.build("a");
    DFA dfa = pdka_builder.build(nka);

    DFA_State* start = dfa.get_state(0);
    ASSERT_NE(start, nullptr);

    auto transition = start->get_neighbour('a');
    EXPECT_TRUE(transition.has_value());
}

TEST_F(PDKATest, BuildDFAFromConcatenation) {
    Tree nka = nka_builder.build("ab.");
    DFA dfa = pdka_builder.build(nka);
    
    EXPECT_GE(dfa.get_states_count(), 2);

    DFA_State* current = dfa.get_state(dfa.get_start_state());
    
    auto transition_a = current->get_neighbour('a');
    EXPECT_TRUE(transition_a.has_value());
}

TEST_F(PDKATest, BuildDFAFromUnion) {
    Tree nka = nka_builder.build("ab+");
    DFA dfa = pdka_builder.build(nka);

    DFA_State* start = dfa.get_state(dfa.get_start_state());
    
    auto transition_a = start->get_neighbour('a');
    auto transition_b = start->get_neighbour('b');
    
    EXPECT_TRUE(transition_a.has_value() || transition_b.has_value());
}

TEST_F(PDKATest, BuildDFAFromStar) {
    Tree nka = nka_builder.build("a*");
    DFA dfa = pdka_builder.build(nka);

    DFA_State* start = dfa.get_state(dfa.get_start_state());
    EXPECT_TRUE(start->is_finish());

    auto transition = start->get_neighbour('a');
    EXPECT_TRUE(transition.has_value());
}

TEST_F(PDKATest, BuildDFAWithEpsilonTransitions) {
    Tree nka;
    State* s0 = new State();
    State* s1 = new State();
    State* s2 = new State();
    State* s3 = new State();
    
    s0->add_link('\0', s1);
    s0->add_link('\0', s2);
    s1->add_link('a', s3);
    s2->add_link('b', s3);
    s3->set_finish(true);
    
    nka.buffer = {s0, s1, s2, s3};
    nka.start = s0->get_id();
    nka.ends = {s3->get_id()};
    
    DFA dfa = pdka_builder.build(nka);

    DFA_State* start = dfa.get_state(dfa.get_start_state());
    
    EXPECT_TRUE(start->get_neighbour('a').has_value() || start->get_neighbour('b').has_value());
}

TEST_F(PDKATest, CompleteDFAHasTransitionsForAllSymbols) {
    Tree nka = nka_builder.build("a");
    DFA dfa = pdka_builder.build(nka);

    for (size_t i = 0; i < dfa.get_states_count(); ++i) {
        DFA_State* state = dfa.get_state(i);
        for (char symbol : alphabet) {
            EXPECT_TRUE(state->get_neighbour(symbol).has_value())
                << "State " << i << " missing transition for symbol '" << symbol << "'";
        }
    }
}

TEST_F(PDKATest, ComplexExpressionDFA) {
    Tree nka = nka_builder.build("ab+*ab.");
    DFA dfa = pdka_builder.build(nka);
    
    EXPECT_GE(dfa.get_states_count(), 2);

    for (size_t i = 0; i < dfa.get_states_count(); ++i) {
        DFA_State* state = dfa.get_state(i);
        for (char symbol : alphabet) {
            EXPECT_TRUE(state->get_neighbour(symbol).has_value())
                << "State " << i << " missing transition for symbol '" << symbol << "'";
        }
    }
}

TEST_F(PDKATest, DFAWithMultipleFinalStates) {
    Tree nka;
    State* s0 = new State();
    State* s1 = new State();
    State* s2 = new State();
    
    s0->add_link('a', s1);
    s0->add_link('b', s2);
    s1->set_finish(true);
    s2->set_finish(true);
    
    nka.buffer = {s0, s1, s2};
    nka.start = s0->get_id();
    nka.ends = {s1->get_id(), s2->get_id()};
    
    DFA dfa = pdka_builder.build(nka);

    EXPECT_GT(dfa.get_final_states().size(), 0);
}

TEST_F(PDKATest, EmptyLanguageDFA) {
    Tree nka;
    State* s0 = new State();
    nka.buffer = {s0};
    nka.start = s0->get_id();
    nka.ends = {};
    
    DFA dfa = pdka_builder.build(nka);

    EXPECT_GE(dfa.get_states_count(), 1);
    EXPECT_EQ(dfa.get_final_states().size(), 0);
}

TEST_F(PDKATest, PrintDFADoesNotCrash) {
    Tree nka = nka_builder.build("ab+*");
    DFA dfa = pdka_builder.build(nka);

    EXPECT_NO_THROW(dfa.print_DFA());
}

TEST_F(PDKATest, DFATask15MethodWorks) {
    Tree nka = nka_builder.build("ab.");
    DFA dfa = pdka_builder.build(nka);

    EXPECT_NO_THROW({
        dfa.Task_15("ab");
        dfa.Task_15("a");
        dfa.Task_15("abc");
        dfa.Task_15("b");
    });
}

TEST_F(PDKATest, DifferentAlphabet) {
    std::vector<char> custom_alphabet = {'x', 'y'};
    NKA_Builder custom_nka_builder(custom_alphabet);
    PDKA_Builder custom_pdka_builder(custom_alphabet);

    Tree nka = custom_nka_builder.build("xy.");
    DFA dfa = custom_pdka_builder.build(nka);
    
    EXPECT_GE(dfa.get_states_count(), 2);
}

TEST_F(PDKATest, MultipleSymbolTransitions) {
    Tree nka = nka_builder.build("ab.c.");
    DFA dfa = pdka_builder.build(nka);

    EXPECT_NO_THROW({
        dfa.Task_15("abc");
        dfa.Task_15("ab");
        dfa.Task_15("abcd");
    });
}

TEST_F(PDKATest, ComplexNFAtoDFAConversion) {
    Tree nka = nka_builder.build("a*b.c+*");
    DFA dfa = pdka_builder.build(nka);

    EXPECT_GE(dfa.get_states_count(), 1);
    EXPECT_EQ(dfa.get_start_state(), 0);

    EXPECT_GT(dfa.get_final_states().size(), 0);
}

TEST_F(PDKATest, SimpleExpressionWithEmptyWord) {
    Tree nka = nka_builder.build("1");
    DFA dfa = pdka_builder.build(nka);

    EXPECT_GE(dfa.get_states_count(), 1);
    EXPECT_GT(dfa.get_final_states().size(), 0);
}

TEST_F(PDKATest, ComplexUnionExpression) {
    Tree nka = nka_builder.build("a*b.c+");
    DFA dfa = pdka_builder.build(nka);

    EXPECT_GE(dfa.get_states_count(), 2);

    for (size_t i = 0; i < dfa.get_states_count(); ++i) {
        DFA_State* state = dfa.get_state(i);
        for (char symbol : alphabet) {
            EXPECT_TRUE(state->get_neighbour(symbol).has_value());
        }
    }
}

TEST_F(PDKATest, VerySimpleTest) {
    Tree nka = nka_builder.build("a");
    DFA dfa = pdka_builder.build(nka);
    EXPECT_GE(dfa.get_states_count(), 1);
}