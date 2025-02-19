package automata;

import java.util.*;

public class DFABuilder {

    public static DFA createIdentifierDFA() {
        DFA dfa = new DFA();
        dfa.setStartState(0);

        for (char c = 'a'; c <= 'z'; c++) {
            dfa.addTransition(0, c, 1);
            dfa.addTransition(1, c, 1);
        }

        dfa.setFinalState(1);
        return dfa;
    }

    public static DFA createNumberDFA() {
        DFA dfa = new DFA();
        dfa.setStartState(0);

        // Digits (0-9) transition to state 1
        for (char c = '0'; c <= '9'; c++) {
            dfa.addTransition(0, c, 1);
            dfa.addTransition(1, c, 1);
        }

        // Decimal point (.) transitions from state 1 to state 2
        dfa.addTransition(1, '.', 2);

        // Digits after decimal (to state 3)
        for (char c = '0'; c <= '9'; c++) {
            dfa.addTransition(2, c, 3);
            dfa.addTransition(3, c, 3);
        }

        //  Mark valid final states (integers and decimals)
        dfa.setFinalState(1); // Whole numbers (e.g., 10)
        dfa.setFinalState(3); // Decimal numbers (e.g., 5.25)
        return dfa;
    }



    public static DFA createOperatorDFA() {
        DFA dfa = new DFA();
        dfa.setStartState(0);

        char[] operators = {'=','+', '-', '*', '/', '%', '^'};
        for (char op : operators) {
            dfa.addTransition(0, op, 1);
        }

        dfa.setFinalState(1);
        return dfa;
    }
}
