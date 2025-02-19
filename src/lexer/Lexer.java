package lexer;

import automata.DFA;
import automata.DFABuilder;
import symbols.SymbolTable;
import symbols.SymbolTableEntry;
import java.util.*;

public class Lexer {

    //  Token types
    public enum TokenType {
        KEYWORD, IDENTIFIER, NUMBER, OPERATOR, SEPARATOR, STRING, COMMENT, CONSTANT, FUNCTION, INVALID
    }

    // Token class
    public static class Token {
        public final TokenType type;
        public final String value;

        public Token(TokenType type, String value) {
            this.type = type;
            this.value = value;
        }

        @Override
        public String toString() {
            return "Token{" + "type=" + type + ", value='" + value + "'}";
        }
    }

    private List<Token> tokens;
    private DFA identifierDFA;
    private DFA numberDFA;
    private DFA operatorDFA;
    private SymbolTable symbolTable;
    private Set<String> keywords;

    // Initialize DFAs & Symbol Table
    public Lexer() {
        this.tokens = new ArrayList<>();
        this.symbolTable = new SymbolTable();

        //  Updated Keywords (`int` → `wn`, `func` → `function`)
        keywords = new HashSet<>(Arrays.asList("wn", "decimal", "var", "if", "else", "while", "print", "input", "return", "true", "false", "function"));

        //  Initialize DFAs
        identifierDFA = DFABuilder.createIdentifierDFA();
        numberDFA = DFABuilder.createNumberDFA();
        operatorDFA = DFABuilder.createOperatorDFA();
    }

    //  Process input code and generate tokens
    public List<Token> process(String input) {
        int index = 0;
        String lastIdentifier = null;
        String lastDataType = null; 
        boolean expectingFunctionName = false; // ✅ Track function name parsing

        while (index < input.length()) {
            char currentChar = input.charAt(index);

            if (Character.isWhitespace(currentChar)) {
                index++;
                continue;
            }

         //  Handle comments (Single-line `$$` and Multi-line `#$ ... $#`)
            if (currentChar == '$' && index + 1 < input.length()) {
                if (input.charAt(index + 1) == '$') {  // ✅ Single-line comment (`$$`)
                    index = consumeSingleLineComment(input, index);
                    tokens.add(new Token(TokenType.COMMENT, "$$..."));
                    continue;
                }
            } 
            else if (currentChar == '#' && index + 1 < input.length()) {
                if (input.charAt(index + 1) == '$') {  // ✅ Multi-line comment (`#$ ... $#`)
                    index = consumeMultiLineComment(input, index);
                    tokens.add(new Token(TokenType.COMMENT, "#$...#$"));
                    continue;
                }
            }


            //  Handle Strings (Detect `"..."`)
            if (currentChar == '"') {
                int end = consumeString(input, index);
                if (end > index) {
                    tokens.add(new Token(TokenType.STRING, input.substring(index, end)));
                    index = end;
                    continue;
                }
            }

            //  Handle Identifiers (Variables & Functions)
            int end = matchDFA(identifierDFA, input, index);
            if (end > index) {
                String tokenValue = input.substring(index, end);
                TokenType type = keywords.contains(tokenValue) ? TokenType.KEYWORD : TokenType.IDENTIFIER;
                tokens.add(new Token(type, tokenValue));

                //  Detect function declaration (`function myFunction() {`)
                if (tokenValue.equals("function")) {  // 🔄 Changed "func" → "function"
                    expectingFunctionName = true;
                }
                //  Store function name in the Symbol Table as a FUNCTION
                else if (expectingFunctionName) {
                    symbolTable.insert(tokenValue, "function", SymbolTableEntry.SymbolType.FUNCTION, "GLOBAL", null);
                    expectingFunctionName = false;
                }
                //  Handle Variable Declarations (`wn x; decimal y;`)
                else if (tokenValue.equals("wn") || tokenValue.equals("decimal")) {  // 🔄 Changed "int" → "wn"
                    lastDataType = tokenValue;
                }
                //  Assign variable type if declared (`wn x; decimal y;`)
                else if (type == TokenType.IDENTIFIER) {
                    if (lastDataType != null) {
                        symbolTable.insert(tokenValue, lastDataType, SymbolTableEntry.SymbolType.VARIABLE, "LOCAL", null);
                        lastDataType = null;
                    } else {
                        symbolTable.insert(tokenValue, "unknown", SymbolTableEntry.SymbolType.VARIABLE, "LOCAL", null);
                    }
                    lastIdentifier = tokenValue;
                }

                index = end;
                continue;
            }

            //  Handle Numbers (Including Decimals)
            end = matchDFA(numberDFA, input, index);
            if (end > index) {
                String numberValue = input.substring(index, end);

                //  Ensure decimal number is valid (not ending in ".")
                if (numberValue.endsWith(".")) {
                    tokens.add(new Token(TokenType.INVALID, "."));
                    index++;
                } else {
                    tokens.add(new Token(TokenType.CONSTANT, numberValue));

                    //  Store constant in the Symbol Table
                    symbolTable.insert(numberValue, "number", SymbolTableEntry.SymbolType.CONSTANT, "GLOBAL", numberValue);
                }

                index = end;
                continue;
            }

            //  Handle Operators
            end = matchDFA(operatorDFA, input, index);
            if (end > index) {
                tokens.add(new Token(TokenType.OPERATOR, input.substring(index, end)));
                index = end;
                continue;
            }

            //  Handle `{` and `}` for function scope
            if (currentChar == '{' || currentChar == '}') {
                tokens.add(new Token(TokenType.SEPARATOR, String.valueOf(currentChar)));
                index++;
                continue;
            }

            //  Handle Separators (`;`, `(`, `)`)
            if (";()".indexOf(currentChar) != -1) {
                tokens.add(new Token(TokenType.SEPARATOR, String.valueOf(currentChar)));
                index++;
                continue;
            }

            // ❌ Handle Invalid Tokens
            tokens.add(new Token(TokenType.INVALID, String.valueOf(currentChar)));
            index++;
        }
        return tokens;
    }

    // Match DFA from a given position in input
    private int matchDFA(DFA dfa, String input, int start) {
        int end = start;
        while (end < input.length() && dfa.simulate(input.substring(start, end + 1))) {
            end++;
        }
        return end;
    }

 //  Consume single-line comments (`$$ ...`)
    private int consumeSingleLineComment(String input, int index) {
        index += 2; // Skip `$$`
        while (index < input.length() && input.charAt(index) != '\n') {
            index++;
        }
        return index;
    }

    //  Consume multi-line comments (`#$ ... $#`)
    private int consumeMultiLineComment(String input, int index) {
        index += 2; // Skip `#$`
        while (index < input.length() - 1 && !(input.charAt(index) == '$' && input.charAt(index + 1) == '#')) {
            index++;
        }
        return index + 2; // Skip `#$ ... $#`
    }


    //  Consume string literals (`"..."`)
    private int consumeString(String input, int index) {
        int start = index;
        index++;
        while (index < input.length() && input.charAt(index) != '"') {
            index++;
        }
        return index < input.length() ? index + 1 : start;
    }

    //  Display tokens
    public void displayTokens() {
        for (Token token : tokens) {
            System.out.println("Token Type: " + token.type + ", Value: " + token.value);
        }
        symbolTable.display(); //  Display Symbol Table
    }
}
