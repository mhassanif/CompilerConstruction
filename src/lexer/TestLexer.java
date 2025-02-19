package lexer;

public class TestLexer {
    public static void main(String[] args) {
        //  Sample code using `{}` for function scope
    	String code = """
    		    function myfunction() {
    		        wn x = 10;
    		        decimal y = x + 525;
    		        print(y);
    		    }
    		    """;

        //  Create Lexer and process input
        Lexer lexer = new Lexer();
        lexer.process(code);

        //  Display the detected tokens
        System.out.println("\n=== Tokenized Output ===");
        lexer.displayTokens();
    }
}
