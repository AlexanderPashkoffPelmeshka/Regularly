#include "Machine.h"
#include <gtest/gtest.h>

class MachineTest : public ::testing::Test {
protected:
    void SetUp() override {
        alphabet = {'a', 'b', 'c'};
    }

    void TearDown() override {
    }

    std::vector<char> alphabet;
};

TEST_F(MachineTest, MachineInitializationWithSingleAlphabet) {
    Machine<> machine(alphabet);
    SUCCEED();
}

TEST_F(MachineTest, MachineInitializationWithMultipleAlphabets) {
    std::vector<char> alphabet1 = {'a', 'b'};
    std::vector<char> alphabet2 = {'a', 'b', 'c'};
    std::vector<char> alphabet3 = {'x', 'y', 'z'};
    
    Machine<> machine(alphabet1, alphabet2, alphabet3);
    SUCCEED();
}

TEST_F(MachineTest, BuildNKAFromSimpleExpression) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        Tree nka = machine.Build_nka("a");
        EXPECT_FALSE(nka.buffer.empty());
    });
}

TEST_F(MachineTest, BuildNKAFromConcatenation) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        Tree nka = machine.Build_nka("ab.");
        EXPECT_FALSE(nka.buffer.empty());
    });
}

TEST_F(MachineTest, BuildNKAFromUnion) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        Tree nka = machine.Build_nka("ab+");
        EXPECT_FALSE(nka.buffer.empty());
    });
}

TEST_F(MachineTest, BuildNKAFromStar) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        Tree nka = machine.Build_nka("a*");
        EXPECT_FALSE(nka.buffer.empty());
    });
}

TEST_F(MachineTest, BuildNKAFromEmptyWord) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        Tree nka = machine.Build_nka("1");
        EXPECT_FALSE(nka.buffer.empty());
    });
}

TEST_F(MachineTest, BuildPDKAFromSimpleExpression) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        DFA dfa = machine.Build_pdka("a");
        EXPECT_GE(dfa.get_states_count(), 1);
    });
}

TEST_F(MachineTest, BuildPDKAFromComplexExpression) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        DFA dfa = machine.Build_pdka("a*b.c+");
        EXPECT_GE(dfa.get_states_count(), 1);
    });
}

TEST_F(MachineTest, BuildMPDKAFromSimpleExpression) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        DFA min_dfa = machine.Build_mpdka("a");
        EXPECT_GE(min_dfa.get_states_count(), 1);
    });
}

TEST_F(MachineTest, BuildMPDKAFromComplexExpression) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        DFA min_dfa = machine.Build_mpdka("ab+*ab.");
        EXPECT_GE(min_dfa.get_states_count(), 1);
    });
}

TEST_F(MachineTest, StarOperationBehavior) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        DFA dfa = machine.Build_pdka("a*");
        dfa.Task_15("");
        dfa.Task_15("a");
        dfa.Task_15("aaa");
        dfa.Task_15("b");
    });
}

TEST_F(MachineTest, UnionOperationBehavior) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        DFA dfa = machine.Build_pdka("ab+");
        dfa.Task_15("a");
        dfa.Task_15("b");
        dfa.Task_15("c");
        dfa.Task_15("ab");
    });
}

TEST_F(MachineTest, IntegrationWithRealUseCase) {
    Machine<> machine(alphabet);

    EXPECT_NO_THROW({
        DFA min_dfa = machine.Build_mpdka("ab+*ab.");
        min_dfa.Task_15("ab");
        min_dfa.Task_15("aab");
        min_dfa.Task_15("bbab");
        min_dfa.Task_15("a");
        min_dfa.Task_15("aba");
        min_dfa.Task_15("b");
    });
}

TEST_F(MachineTest, VerySimpleTest) {
    Machine<> machine(alphabet);
    EXPECT_NO_THROW(machine.Build_nka("a"));
}

TEST_F(MachineTest, AnotherSimpleTest) {
    Machine<> machine(alphabet);
    EXPECT_NO_THROW(machine.Build_pdka("a"));
}

TEST_F(MachineTest, YetAnotherSimpleTest) {
    Machine<> machine(alphabet);
    EXPECT_NO_THROW(machine.Build_mpdka("a"));
}

TEST_F(MachineTest, ComplexNestedExpression) {
    Machine<> machine(alphabet);
    
    EXPECT_NO_THROW({
        Tree nka = machine.Build_nka("ab.*c+*");
        EXPECT_FALSE(nka.buffer.empty());
    });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}