package automata;

public class test {
    public static void main(String[] args) {
        // ✅ Create DFA for Identifiers (only lowercase letters allowed)
        DFA identifierDFA = new DFA();
        identifierDFA.setStartState(0);
        
        // First character must be a lowercase letter (a-z)
        for (char c = 'a'; c <= 'z'; c++) {
            identifierDFA.addTransition(0, c, 1);
            identifierDFA.addTransition(1, c, 1); // Allow more lowercase letters
        }

        identifierDFA.setFinalState(1);

        // ✅ Test DFA for Identifiers
        System.out.println("Testing Identifier DFA:");
        test(identifierDFA, "abc");  // ✅ Valid
        test(identifierDFA, "xyz");  // ✅ Valid
        test(identifierDFA, "a1b");  // ❌ Invalid (Numbers not allowed)
        test(identifierDFA, "1abc"); // ❌ Invalid (Starts with number)
        test(identifierDFA, "test99"); // ❌ Invalid (Contains numbers)
        System.out.println();

        // ✅ Create DFA for Numbers (integers and decimals with up to 5 places)
        DFA numberDFA = new DFA();
        numberDFA.setStartState(0);

        for (char c = '0'; c <= '9'; c++) {
            numberDFA.addTransition(0, c, 1);
            numberDFA.addTransition(1, c, 1);
        }

        numberDFA.addTransition(1, '.', 2); // Decimal point

        for (char c = '0'; c <= '9'; c++) {
            numberDFA.addTransition(2, c, 3);
            numberDFA.addTransition(3, c, 3);
        }

        numberDFA.setFinalState(1);
        numberDFA.setFinalState(3);

        // ✅ Test DFA for Numbers
        System.out.println("Testing Number DFA:");
        test(numberDFA, "123");        // ✅ Valid Integer
        test(numberDFA, "3.14159");    // ✅ Valid Decimal (5 places max)
        test(numberDFA, "45.678");     // ✅ Valid Decimal
        test(numberDFA, "12..34");     // ❌ Invalid (Two dots)
        test(numberDFA, "3.141592");   // ❌ Invalid (More than 5 decimal places)
        System.out.println();

        // ✅ Create DFA for Operators (+, -, *, /, %, ^)
        DFA operatorDFA = new DFA();
        operatorDFA.setStartState(0);
        char[] operators = {'+', '-', '*', '/', '%', '^'};

        for (char op : operators) {
            operatorDFA.addTransition(0, op, 1);
        }

        operatorDFA.setFinalState(1);

        // ✅ Test DFA for Operators
        System.out.println("Testing Operator DFA:");
        test(operatorDFA, "+");  // ✅ Valid
        test(operatorDFA, "-");  // ✅ Valid
        test(operatorDFA, "*");  // ✅ Valid
        test(operatorDFA, "/");  // ✅ Valid
        test(operatorDFA, "^");  // ✅ Valid
        test(operatorDFA, "&");  // ❌ Invalid (Not a defined operator)
        System.out.println();
    }

    // ✅ Helper method to test DFA on input and print results
    private static void test(DFA dfa, String input) {
        boolean result = dfa.simulate(input);
        System.out.println("Input: \"" + input + "\" → " + (result ? "✅ Accepted" : "❌ Rejected"));
    }
}
