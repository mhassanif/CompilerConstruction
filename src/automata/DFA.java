package automata;

import java.util.*;

public class DFA {
    private Set<Integer> states;  // Set of all states
    private int startState;       // Start state
    private Set<Integer> finalStates;  // Set of final states
    private Map<Integer, Map<Character, Integer>> transitionTable; // Transition table

    //  Constructor to initialize DFA
    public DFA() {
        this.states = new HashSet<>();
        this.finalStates = new HashSet<>();
        this.transitionTable = new HashMap<>();
    }

    //  Set the start state
    public void setStartState(int start) {
        this.startState = start;
        this.states.add(start);
    }

    //  Add a transition from one state to another on a given character
    public void addTransition(int from, char input, int to) {
        transitionTable.putIfAbsent(from, new HashMap<>());
        transitionTable.get(from).put(input, to);
        states.add(from);
        states.add(to);
    }

    public void setFinalState(int state) {
        this.finalStates.add(state);
        this.states.add(state);
    }

    public boolean simulate(String input) {
        int currentState = startState;

        for (char symbol : input.toCharArray()) {
            if (!transitionTable.containsKey(currentState) || !transitionTable.get(currentState).containsKey(symbol)) {
                return false; // Reached a dead state
            }
            currentState = transitionTable.get(currentState).get(symbol);
        }
        return finalStates.contains(currentState); // Accept if in final state
    }

    public void displayDFA() {
        System.out.println("DFA States:");
        for (Integer state : states) {
            System.out.println("State " + state + (finalStates.contains(state) ? " (Final)" : ""));
            if (transitionTable.containsKey(state)) {
                for (Map.Entry<Character, Integer> entry : transitionTable.get(state).entrySet()) {
                    System.out.println("  " + state + " --" + entry.getKey() + "--> " + entry.getValue());
                }
            }
        }
    }

    // Getters
    public int getStartState() {
        return startState;
    }

    public Set<Integer> getFinalStates() {
        return finalStates;
    }

    public Map<Integer, Map<Character, Integer>> getTransitionTable() {
        return transitionTable;
    }
}
