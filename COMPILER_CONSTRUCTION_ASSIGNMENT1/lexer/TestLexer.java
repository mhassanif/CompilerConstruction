package lexer;

public class TestLexer {
    public static void main(String[] args) {
        String code = """
                function myfunction() {
                hfa;
                    wn x2 = 10;
                    decimal y = 53 &;
                    print(y);
                    &;
                }
                $$ This is a comment
                #$ This is a 
                multi-line comment $#
                %;
                
                """;

        // Create Lexer and process input
        Lexer lexer = new Lexer();

      /*  System.out.println("\n=== Displaying DFAs ===");
        lexer.identifierDFA.displayDFA();
        lexer.numberDFA.displayDFA();
        lexer.operatorDFA.displayDFA();
*/
        // Process the input code
        lexer.process(code);

        //  Display the detected tokens
        System.out.println("\n==========");
        lexer.displayTokens();
    }
}
