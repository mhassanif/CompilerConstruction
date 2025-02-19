package automata;

import java.util.*;

public class State {
    private static int counter = 0;
    private int id;
    private boolean isFinal;
    private List<Transition> transitions;

    public State() {
        this.id = counter++;
        this.isFinal = false;
        this.transitions = new ArrayList<>();
    }

    public State(boolean isFinal) {
        this();
        this.isFinal = isFinal;
    }

    public int getId() {
        return id;
    }

    public boolean isFinal() {
        return isFinal;
    }

    public void setFinal(boolean isFinal) {
        this.isFinal = isFinal;
    }

    public void addTransition(Transition transition) {
        transitions.add(transition);
    }

    public List<Transition> getTransitions() {
        return transitions;
    }

}
