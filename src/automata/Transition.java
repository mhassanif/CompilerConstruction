package automata;

public class Transition {
    private char symbol;
    private State to;

    public Transition(char symbol, State to) {
        this.symbol = symbol;
        this.to = to;
    }

    public char getSymbol() {
        return symbol;
    }

    public State getTo() {
        return to;
    }
}
