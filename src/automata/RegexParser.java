package automata;

import java.util.Stack;

public class RegexParser {
    private String regex;

    // Constructor to accept a regex string
    public RegexParser(String regex) {
        this.regex = regex;
    }

    // Convert regex to NFA using Thompson's construction
    public NFA toNFA() {
        Stack<NFA> stack = new Stack<>();

        for (char c : regex.toCharArray()) {
            if (c == '*') {
                NFA nfa = stack.pop();
                stack.push(NFA.star(nfa));
            } else if (c == '|') {
                NFA nfa2 = stack.pop();
                NFA nfa1 = stack.pop();
                stack.push(NFA.union(nfa1, nfa2));
            } else if (c == '.') {
                NFA nfa2 = stack.pop();
                NFA nfa1 = stack.pop();
                stack.push(NFA.concat(nfa1, nfa2));
            } else {
                stack.push(NFA.singleChar(c));
            }
        }

        return stack.pop();
    }
}
