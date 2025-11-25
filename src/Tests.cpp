#include "Getter_Grammatic.h"
#include <cassert>
#include <stdexcept>

void Test1() {
    std::cout << "Запуск...\n";
    
    {
        Erly_Machine machine;
        try {
            machine.Set_Alphabet("");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "1\n";
        }
    }

    {
        Erly_Machine machine;
        try {
            machine.Set_Alphabet("aa");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "2\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        try {
            machine.Add_Link("S -> x");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "3\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        try {
            machine.Add_Link(" -> a");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "4\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        try {
            machine.Add_Link("SS -> a");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "5\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        try {
            machine.Add_Link("X -> a");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "6\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        try {
            machine.Add_Link("a -> S");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "7\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        try {
            machine.Add_Link("S a");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "8\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("ab");
        machine.Set_Neterminals("S");
        machine.Add_Link("S -> a");
        
        try {
            machine.Get_word("abc");
            assert(false);
}        catch (const std::invalid_argument& e) {
            std::cout << "9\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        
        try {
            machine.Get_word("a");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "10\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        
        try {
            machine.Get_word("a");
            assert(false);
        } catch (const std::invalid_argument& e) {
            std::cout << "11\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        machine.Add_Link("S -> a");
        machine.Add_Link("S -> a S");
        
        std::string long_word(100, 'a');
        try {
            machine.Get_word(long_word);
            assert(machine.is_word_in_grammar());
            std::cout << "12\n";
        } catch (...) {
            std::cout << "999";
        }
    }   

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("SABCD");
        machine.Add_Link("S -> A B C D");
        machine.Add_Link("A -> ");
        machine.Add_Link("B -> ");
        machine.Add_Link("C -> ");
        machine.Add_Link("D -> ");
        
        machine.Get_word("");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "13\n";
        } else {
            std::cout << "999";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("()a");
        machine.Set_Neterminals("S");
        machine.Add_Link("S -> a");
        machine.Add_Link("S -> S S");
        machine.Add_Link("S -> ( S )");
        
        
        machine.Get_word("((((a))))");
        if (machine.is_word_in_grammar()) {
            std::cout << "14\n";
        } else {
            std::cout << "999";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        machine.Add_Link("S -> a");
        
        machine.Get_word("");
        if (machine.is_word_in_grammar() == false) {
            std::cout << "1";
        } else {
            std::cout << "999";
        }
        
        machine.Get_word("a");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "5\n"; // ну типа, если сделаются два, то получится 15 :)
        } else {
            std::cout << "999";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a+-*/()");
        machine.Set_Neterminals("S");
        machine.Add_Link("S -> ( S )");
        machine.Add_Link("S -> S + S");
        machine.Add_Link("S -> S * S");
        machine.Add_Link("S -> a");

        machine.Get_word("(a+a)*a");
        if (machine.is_word_in_grammar()) {
            std::cout << "16\n";
        } else {
            std::cout << "999\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("SAB");
        machine.Add_Link("S -> A");
        machine.Add_Link("A -> B");
        machine.Add_Link("B -> S");
        machine.Add_Link("S -> B");
        machine.Add_Link("S -> a");

        machine.Get_word("a");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "17\n";
        } else {
            std::cout << "999\n";
        }
    }

    {
        Erly_Machine machine1, machine2;
        
        machine1.Set_Alphabet("a");
        machine1.Set_Neterminals("S");
        machine1.Add_Link("S -> a");
        
        machine2.Set_Alphabet("b");
        machine2.Set_Neterminals("T");
        machine2.Add_Link("T -> b");
        
        machine1.Get_word("a");
        machine2.Get_word("b");
        
        if (machine1.is_word_in_grammar() == true && machine2.is_word_in_grammar() == true) {
            std::cout << "18\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("ab");
        machine.Set_Neterminals("S");
        machine.Add_Link("S -> a S b");
        machine.Add_Link("S -> ");
        
        machine.Get_word("ab");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "1";
        }
        
        machine.Get_word("aabb");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "9";
        }
        
        machine.Get_word("a");
        if (machine.is_word_in_grammar() == false) {
            std::cout << "\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("abcdefghij");
        machine.Set_Neterminals("SABCDEFGHIJ");
        char t = 96;
        for (char nt = 'A'; nt <= 'J'; ++nt) {
            ++t;
            std::string rule = std::string(1, nt) + " -> " + t;
            // std::cout << rule << "\n";
            machine.Add_Link(rule);
        }
        machine.Add_Link("S -> A B C D E F G H I J");
        
        machine.Get_word("abcdefghij");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "20\n";
        } else {
            std::cout << "999\n";
        }
    }

}

void Test2() {
    
    {
        Erly_Machine machine;
        machine.Set_Alphabet("a");
        machine.Set_Neterminals("S");
        machine.Add_Link("S -> a");
        
        machine.Get_word("a");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "2";
        } else {
            std::cout << "999\n";
        }
        
        machine.Get_word("");
        if (machine.is_word_in_grammar() == false) {
            std::cout << "1";
        } else {
            std::cout << "999\n";
        }
        
        machine.Get_word("aa");
        if (machine.is_word_in_grammar() == false) {
            std::cout << "\n";
        } else {
            std::cout << "999\n";
        }
    }
    
    {
        Erly_Machine machine;
        machine.Set_Alphabet("ab");
        machine.Set_Neterminals("SAB");
        machine.Add_Link("S -> A B");
        machine.Add_Link("A -> a");
        machine.Add_Link("B -> b");
        
        machine.Get_word("ab");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "2";
        }  else {
            std::cout << "999\n";
        }
        
        machine.Get_word("a");
        if (machine.is_word_in_grammar() == false) {
            std::cout << "2";
        } else {
            std::cout << "999\n";
        }
        
        machine.Get_word("b");
        if (machine.is_word_in_grammar() == false) {
            std::cout << "\n";
        } else {
            std::cout << "999\n";
        }
    }

    {
        Erly_Machine machine;
        machine.Set_Alphabet("abc");
        machine.Set_Neterminals("SABC");
        machine.Add_Link("S -> A B C");
        machine.Add_Link("A -> a");
        machine.Add_Link("B -> b");
        machine.Add_Link("C -> c");
        machine.Add_Link("A -> ");
        machine.Add_Link("B -> ");
        machine.Add_Link("C -> ");
        
        machine.Get_word("abc");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "2";
        }
        
        machine.Get_word("");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "3";
        }
        
        machine.Get_word("a");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "!";
        }
        
        machine.Get_word("ab");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "\n";
        }
    }


    {
        Erly_Machine machine; // этот пример из домашки XD
        machine.Set_Alphabet("abc");
        machine.Set_Neterminals("SABCDE");
        machine.Add_Link("S -> AcB");
        machine.Add_Link("A -> CcC");
        machine.Add_Link("A -> B");
        machine.Add_Link("A ->");
        machine.Add_Link("B -> cD");
        machine.Add_Link("B -> b");
        machine.Add_Link("C -> a");
        machine.Add_Link("C -> cC");
        machine.Add_Link("D -> CD");
        machine.Add_Link("D -> b");
        machine.Add_Link("E -> aC");
        machine.Add_Link("E -> BD");
        
        machine.Get_word("acacb");
        if (machine.is_word_in_grammar() == true) {
            std::cout << "Домашка верна!!";
        } else {
            std::cout << "УУУ, ЛАЖА!!!";
        }
    }    
}

int main() {
    try {
        Test1();
        std::cout << "\n";
        Test2();
        std::cout << "\nПобеда\n";
    } catch (const std::exception& e) {
        std::cout << "999\n";
        return 1;
    }
    
    return 0;
}
