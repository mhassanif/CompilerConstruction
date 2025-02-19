package automata;

import java.util.*;

public class NFA {
    private State startState;
    private Set<State> finalStates;
    private Set<State> states; // ✅ Track all states in the NFA

    public NFA(State startState, Set<State> finalStates) {
        this.startState = startState;
        this.finalStates = finalStates;
        this.states = new HashSet<>();
        collectStates(startState);
    }

    public State getStartState() {
        return startState;
    }

    public Set<State> getFinalStates() {
        return finalStates;
    }

    public Set<State> getStates() { // ✅ Needed for DFA conversion
        return states;
    }

    private void collectStates(State state) {
        if (states.contains(state)) return;
        states.add(state);
        for (Transition t : state.getTransitions()) {
            collectStates(t.getTo());
        }
    }

    //  Epsilon closure (used in simulation and DFA conversion)
    private Set<State> epsilonClosure(Set<State> states) {
        Stack<State> stack = new Stack<>();
        stack.addAll(states);
        Set<State> closure = new HashSet<>(states);

        while (!stack.isEmpty()) {
            State state = stack.pop();
            for (Transition t : state.getTransitions()) {
                if (t.getSymbol() == '\0' && !closure.contains(t.getTo())) { // ε-move
                    closure.add(t.getTo());
                    stack.push(t.getTo());
                }
            }
        }
        return closure;
    }

    //  Move function: Finds the next states from a given state on input symbol
    private Set<State> move(Set<State> states, char symbol) {
        Set<State> result = new HashSet<>();
        for (State state : states) {
            for (Transition t : state.getTransitions()) {
                if (t.getSymbol() == symbol) {
                    result.add(t.getTo());
                }
            }
        }
        return result;
    }

    //  Simulates the NFA
    public boolean accepts(String input) {
        Set<State> currentStates = epsilonClosure(Set.of(startState));

        for (char c : input.toCharArray()) {
            currentStates = move(currentStates, c);
            currentStates = epsilonClosure(currentStates);
        }

        for (State state : currentStates) {
            if (finalStates.contains(state)) return true;
        }
        return false;
    }

    //  Thompson’s Construction Helpers

    // Single character NFA
    public static NFA singleChar(char c) {
        State start = new State();
        State end = new State(true);
        start.addTransition(new Transition(c, end));
        return new NFA(start, Set.of(end));
    }

    // Concatenation of two NFAs
    public static NFA concat(NFA nfa1, NFA nfa2) {
        for (State state : nfa1.getFinalStates()) {
            state.addTransition(new Transition('\0', nfa2.getStartState())); // ε-transition
            state.setFinal(false);
        }
        return new NFA(nfa1.getStartState(), nfa2.getFinalStates());
    }

    // Union of two NFAs
    public static NFA union(NFA nfa1, NFA nfa2) {
        State start = new State();
        start.addTransition(new Transition('\0', nfa1.getStartState()));
        start.addTransition(new Transition('\0', nfa2.getStartState()));

        Set<State> finalStates = new HashSet<>(nfa1.getFinalStates());
        finalStates.addAll(nfa2.getFinalStates());

        return new NFA(start, finalStates);
    }

    // Kleene Star (repetition)
    public static NFA star(NFA nfa) {
        State start = new State();
        State end = new State(true);

        start.addTransition(new Transition('\0', nfa.getStartState()));
        start.addTransition(new Transition('\0', end));

        for (State state : nfa.getFinalStates()) {
            state.addTransition(new Transition('\0', nfa.getStartState()));
            state.addTransition(new Transition('\0', end));
            state.setFinal(false);
        }

        return new NFA(start, Set.of(end));
    }
}
