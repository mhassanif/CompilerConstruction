package lexer;

import DFAs.DFA;
import DFAs.DFAB;
import symbols.SymbolTable;
import symbols.SymbolTableEntry;
import java.util.*;

public class Lexer 
{

	// TYPEs of Tokens
    public enum TYPE_OF_TOKEN 
    {
        KEYWORD, IDENTIFIER, NUMBER, OPERATOR, SEPARATOR, STRING, COMMENT, CONSTANT, FUNCTION, INVALID
    }

    // Token class
    public static class Token 
    {
        public final TYPE_OF_TOKEN type;
        public final String value;

        public Token(TYPE_OF_TOKEN type, String value) 
        {
            this.type = type;
            this.value = value;
        }

        @Override
        public String toString() 
        {
            return "Token{" + "type=" + type + ", value='" + value + "'}";
        }
    }

    private List<Token> tokens;
    DFA identifierDFA;
    DFA numberDFA;
    DFA operatorDFA;
   
    private SymbolTable symbolTable;
    
    private Set<String> keywords;
    private List<Token> invalidTokens = new ArrayList<>();

    public Lexer() 
    {
        this.tokens = new ArrayList<>();
        this.symbolTable = new SymbolTable();

        // Updated Keywords (`int` → `wn`, `func` → `function`)
        keywords = new HashSet<>(Arrays.asList("wn", "decimal", "var", "if", "else", "while", "print", "input", "return", "true", "false", "function"));

       
        identifierDFA = DFAB.createIDFA();
        numberDFA = DFAB.createNDFA();
        operatorDFA = DFAB.createODFA();
    }

    // Process input code and generate tokens
    public List<Token> process(String input) 
    {
        int i = 0;
        String LI = null;
        String lastDataType = null;
        boolean expectfun = false; 

        while (i < input.length()) 
        {
           
        	char currentChar = input.charAt(i);

            if (Character.isWhitespace(currentChar))
            {
                i++;
                continue;
            }

//this is to deal with singlle line and multiline comments
            
            if (currentChar == '$' && i + 1 < input.length()) 
            {
                if (input.charAt(i + 1) == '$') 
                {  
                // If found, it consumes the entire comment, adds a COMMENT token, and moves to the next part of the input.	
                    i = consumeSingleLineComment(input, i);
                    tokens.add(new Token(TYPE_OF_TOKEN.COMMENT, "$$..."));
                    continue;
                }
            } 
            
            else if (currentChar == '#' && i + 1 < input.length()) {
                if (input.charAt(i + 1) == '$') 
                {  
                	// If found, it consumes the entire comment, adds a COMMENT token, and moves to the next part of the input.
                    i = Remove_MULTICMNT(input, i);
                    tokens.add(new Token(TYPE_OF_TOKEN.COMMENT, "#$...#$"));
                    continue;
                }
            }

            if (currentChar == '"')
            {
                int end = consumeString(input, i);
                if (end > i) 
                {
                    tokens.add(new Token(TYPE_OF_TOKEN.STRING, input.substring(i, end)));
                    i = end;
                    continue;
                }
            }

            int end = matchDFA(identifierDFA, input, i);
            if (end > i) 
            {
                String tokenValue = input.substring(i, end);
                TYPE_OF_TOKEN type = keywords.contains(tokenValue) ? TYPE_OF_TOKEN.KEYWORD : TYPE_OF_TOKEN.IDENTIFIER;
                tokens.add(new Token(type, tokenValue));

                if (tokenValue.equals("function")) 
                {  
                    expectfun = true;
                }
                else if (expectfun) 
                {
                    symbolTable.insert(tokenValue, "function", SymbolTableEntry.SymbolType.FUNCTION, "GLOBAL", null);
                    expectfun = false;
                }
                else if (tokenValue.equals("wn") || tokenValue.equals("decimal")) {  // 🔄 Changed "int" → "wn"
                    lastDataType = tokenValue;
                }
                else if (type == TYPE_OF_TOKEN.IDENTIFIER) {
                    if (lastDataType != null) {
                        symbolTable.insert(tokenValue, lastDataType, SymbolTableEntry.SymbolType.VARIABLE, "LOCAL", null);
                        lastDataType = null;
                    } else {
                        symbolTable.insert(tokenValue, "unknown", SymbolTableEntry.SymbolType.VARIABLE, "LOCAL", null);
                    }
                    LI = tokenValue;
                }

                i = end;
                continue;
            }

            // This is to handle numbers 
            int end1 = matchDFA(numberDFA, input, i);
            if (end1 > i) 
            {
                String numberValue = input.substring(i, end1);

                if (numberValue.endsWith(".")) 
                {
                    invalidTokens.add(new Token(TYPE_OF_TOKEN.INVALID, ".")); 
                    i = end1;  
                }
                else 
                {
                    tokens.add(new Token(TYPE_OF_TOKEN.CONSTANT, numberValue));

                    symbolTable.insert(numberValue, "number", SymbolTableEntry.SymbolType.CONSTANT, "GLOBAL", numberValue);
                }

                i = end1;
                continue;
            }

            // Handle Operators
            end1 = matchDFA(operatorDFA, input, i);
            if (end1 > i) 
            {
                tokens.add(new Token(TYPE_OF_TOKEN.OPERATOR, input.substring(i, end1)));
                i = end1;
                continue;
            }

            if (currentChar == '{' || currentChar == '}') {
                tokens.add(new Token(TYPE_OF_TOKEN.SEPARATOR, String.valueOf(currentChar)));
                i++;
                continue;
            }

            if (";()".indexOf(currentChar) != -1) {
                tokens.add(new Token(TYPE_OF_TOKEN.SEPARATOR, String.valueOf(currentChar)));
                i++;
                continue;
            }

            Token invalidToken = new Token(TYPE_OF_TOKEN.INVALID, String.valueOf(currentChar));
            tokens.add(invalidToken);       
            invalidTokens.add(invalidToken); 
            i++;
        }
        return tokens;
    }

    private int matchDFA(DFA dfa, String input, int start) {
        int end = start;
        int lastValidEnd = -1;

        while (end < input.length() && dfa.simulate(input.substring(start, end + 1))) {
            lastValidEnd = end + 1;
            end++;
        }

        return (lastValidEnd != -1) ? lastValidEnd : start;
    }

    private int consumeSingleLineComment(String input, int i)
    {
        i += 2; // Skip `$$`
        while (i < input.length() && input.charAt(i) != '\n') 
        {
            i++;
        }
        return i;
    }

    private int Remove_MULTICMNT(String input, int i) 
    {
        i += 2; // Skip `#$`
        while (i < input.length() - 1 && !(input.charAt(i) == '$' && input.charAt(i + 1) == '#')) {
            i++;
        }
        return i + 2; // Skip `#$ ... $#`
    }

    private int consumeString(String input, int i) {
        int start = i;
        i++;
        while (i < input.length() && input.charAt(i) != '"') {
            i++;
        }
        return i < input.length() ? i + 1 : start;
    }

 // 
    public void displayTokens()
    {
        System.out.println("\n-----OUTPUT FOR TOKENS------");

        for (Token token : tokens) 
        {
            if (token.type != TYPE_OF_TOKEN.INVALID) {
                System.out.println("Token Type: " + token.type + ", Value: " + token.value);
            }
        }

      
        symbolTable.display();

        
        System.out.println("\n ------LIST OF ERRORS-------");
        if (invalidTokens.isEmpty()) 
        {
            System.out.println("No errors detected.");
        }
        else 
        {
            for (Token token : invalidTokens)
            {
                System.out.println("Invalid Token: " + token.value);
            }
        }
    }

}
