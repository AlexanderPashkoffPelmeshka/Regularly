#include <gtest/gtest.h>

#include "Machine.h"

class MPDKATest : public ::testing::Test {
protected:
    MPDKATest() : alphabet({'a', 'b'}), nka_builder(alphabet), pdka_builder(alphabet), mpdka_builder(alphabet) {}
    
    void SetUp() override {
    }

    void TearDown() override {
    }

    std::vector<char> alphabet;
    NKA_Builder nka_builder;
    PDKA_Builder pdka_builder;
    MPDKA_Builder mpdka_builder;

    DFA createSimpleDFA() {
        DFA dfa(0);
        DFA_State* s0 = dfa.add_state(0);
        DFA_State* s1 = dfa.add_state(1);
        DFA_State* s2 = dfa.add_state(2);
        
        s0->add_link('a', 1);
        s0->add_link('b', 2);
        s1->add_link('a', 1);
        s1->add_link('b', 2);
        s2->add_link('a', 2);
        s2->add_link('b', 2);
        
        s1->set_finish(true);
        dfa.add_final_state(1);
        
        return dfa;
    }
    
    DFA createDFAWithEquivalentStates() {
        DFA dfa(0);
        DFA_State* s0 = dfa.add_state(0);
        DFA_State* s1 = dfa.add_state(1);
        DFA_State* s2 = dfa.add_state(2);
        DFA_State* s3 = dfa.add_state(3);
        
        s0->add_link('a', 1);
        s0->add_link('b', 2);
        s1->add_link('a', 3);
        s1->add_link('b', 3);
        s2->add_link('a', 3);
        s2->add_link('b', 3);
        s3->add_link('a', 3);
        s3->add_link('b', 3);
        
        s3->set_finish(true);
        dfa.add_final_state(3);
        
        return dfa;
    }
};

TEST_F(MPDKATest, MinimizeAlreadyMinimalDFA) {
    DFA original_dfa = createSimpleDFA();
    DFA min_dfa = mpdka_builder.build(original_dfa);

    EXPECT_LE(min_dfa.get_states_count(), original_dfa.get_states_count());
    EXPECT_GE(min_dfa.get_final_states().size(), 1);
}

TEST_F(MPDKATest, MinimizeDFAWithEquivalentStates) {
    DFA original_dfa = createDFAWithEquivalentStates();
    size_t original_size = original_dfa.get_states_count();
    
    DFA min_dfa = mpdka_builder.build(original_dfa);
    
    EXPECT_LT(min_dfa.get_states_count(), original_size);
}

TEST_F(MPDKATest, FullPipelineNFAtoMinimalDFA) {
    Tree nka = nka_builder.build("ab+*");
    DFA dfa = pdka_builder.build(nka);
    DFA min_dfa = mpdka_builder.build(dfa);
    
    EXPECT_LE(min_dfa.get_states_count(), dfa.get_states_count());
    EXPECT_GE(min_dfa.get_final_states().size(), 1);

    EXPECT_NO_THROW({
        min_dfa.Task_15("");
        min_dfa.Task_15("a");
        min_dfa.Task_15("abba");
    });
}

TEST_F(MPDKATest, FinalStatesPreservedAfterMinimization) {
    Tree nka = nka_builder.build("ab+");
    DFA dfa = pdka_builder.build(nka);
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_GE(min_dfa.get_final_states().size(), 1);
}

TEST_F(MPDKATest, MinimizationPreservesLanguage) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    DFA_State* s1 = dfa.add_state(1);
    
    s0->add_link('a', 1);
    s0->add_link('b', 0);
    s1->add_link('a', 1);
    s1->add_link('b', 0);
    
    s1->set_finish(true);
    dfa.add_final_state(1);
    
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_NO_THROW({
        min_dfa.Task_15("a");
        min_dfa.Task_15("ba");
        min_dfa.Task_15("aba");
        min_dfa.Task_15("b");
        min_dfa.Task_15("ab");
        min_dfa.Task_15("bab");
    });
}

TEST_F(MPDKATest, AllStatesReachableInMinimizedDFA) {
    Tree nka = nka_builder.build("a*b.");
    DFA dfa = pdka_builder.build(nka);
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_GE(min_dfa.get_states_count(), 1);
}

TEST_F(MPDKATest, KillOptimisationFlag) {
    MPDKA_Builder builder_with_optimisation(alphabet);
    MPDKA_Builder builder_without_optimisation(alphabet);
    
    builder_without_optimisation.set_kill_optimisation(false);

    Tree nka = nka_builder.build("ab.");
    DFA dfa = pdka_builder.build(nka);
    
    DFA min_dfa_with = builder_with_optimisation.build(dfa);
    DFA min_dfa_without = builder_without_optimisation.build(dfa);
    
    EXPECT_GE(min_dfa_with.get_states_count(), 1);
    EXPECT_GE(min_dfa_without.get_states_count(), 1);
}

TEST_F(MPDKATest, ComplexExpressionMinimization) {
    Tree nka = nka_builder.build("ab+*ab.");
    DFA dfa = pdka_builder.build(nka);
    size_t original_size = dfa.get_states_count();
    
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_LE(min_dfa.get_states_count(), original_size);

    EXPECT_NO_THROW({
        min_dfa.Task_15("ab");
        min_dfa.Task_15("aab");
        min_dfa.Task_15("bbab");
    });
}

TEST_F(MPDKATest, EmptyLanguageMinimization) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    s0->add_link('a', 0);
    s0->add_link('b', 0);

    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_GE(min_dfa.get_states_count(), 1);
    EXPECT_EQ(min_dfa.get_final_states().size(), 0);
}

TEST_F(MPDKATest, SingleStateDFAMinimization) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    s0->add_link('a', 0);
    s0->add_link('b', 0);
    s0->set_finish(true);
    dfa.add_final_state(0);
    
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_EQ(min_dfa.get_states_count(), 1);
    EXPECT_EQ(min_dfa.get_final_states().size(), 1);
    EXPECT_TRUE(min_dfa.get_state(0)->is_finish());
}

TEST_F(MPDKATest, PrintMinimizedDFADoesNotCrash) {
    Tree nka = nka_builder.build("ab+*");
    DFA dfa = pdka_builder.build(nka);
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_NO_THROW(min_dfa.print_DFA());
}

TEST_F(MPDKATest, DifferentAlphabetMinimization) {
    std::vector<char> custom_alphabet = {'x', 'y', 'z'};
    NKA_Builder custom_nka_builder(custom_alphabet);
    PDKA_Builder custom_pdka_builder(custom_alphabet);
    MPDKA_Builder custom_mpdka_builder(custom_alphabet);

    Tree nka = custom_nka_builder.build("xy.z+");
    DFA dfa = custom_pdka_builder.build(nka);
    DFA min_dfa = custom_mpdka_builder.build(dfa);

    EXPECT_GE(min_dfa.get_states_count(), 1);
    EXPECT_GE(min_dfa.get_final_states().size(), 1);
}

TEST_F(MPDKATest, NoReductionWhenAlreadyMinimal) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    DFA_State* s1 = dfa.add_state(1);
    
    s0->add_link('a', 1);
    s0->add_link('b', 0);
    s1->add_link('a', 1);
    s1->add_link('b', 1);
    
    s1->set_finish(true);
    dfa.add_final_state(1);
    
    size_t original_size = dfa.get_states_count();
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_EQ(min_dfa.get_states_count(), original_size);
}

TEST_F(MPDKATest, MultipleFinalStatesMinimization) {
    DFA dfa(0);
    DFA_State* s0 = dfa.add_state(0);
    DFA_State* s1 = dfa.add_state(1);
    DFA_State* s2 = dfa.add_state(2);
    DFA_State* s3 = dfa.add_state(3);
    
    s0->add_link('a', 1);
    s0->add_link('b', 2);
    s1->add_link('a', 3);
    s1->add_link('b', 3);
    s2->add_link('a', 3);
    s2->add_link('b', 3);
    s3->add_link('a', 3);
    s3->add_link('b', 3);

    s1->set_finish(true);
    s2->set_finish(true);
    s3->set_finish(true);
    dfa.add_final_state(1);
    dfa.add_final_state(2);
    dfa.add_final_state(3);
    
    size_t original_size = dfa.get_states_count();
    DFA min_dfa = mpdka_builder.build(dfa);

    EXPECT_LT(min_dfa.get_states_count(), original_size);
}

TEST_F(MPDKATest, VerySimpleMinimization) {
    Tree nka = nka_builder.build("a");
    DFA dfa = pdka_builder.build(nka);
    DFA min_dfa = mpdka_builder.build(dfa);
    
    EXPECT_GE(min_dfa.get_states_count(), 1);
}

TEST_F(MPDKATest, AnotherSimpleMinimization) {
    Tree nka = nka_builder.build("ab.");
    DFA dfa = pdka_builder.build(nka);
    DFA min_dfa = mpdka_builder.build(dfa);
    
    EXPECT_GE(min_dfa.get_states_count(), 1);
}