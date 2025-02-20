package lexer;

public class Token {
    // Enum for token types
    public enum TokenType {
        KEYWORD, IDENTIFIER, OPERATOR, NUMBER, WHITESPACE, COMMENT, INVALID
    }

    private TokenType type;
    private String lexeme;

    public Token(TokenType type, String lexeme) {
        this.type = type;
        this.lexeme = lexeme;
    }

    public TokenType getType() {
        return type;
    }

    public String getLexeme() {
        return lexeme;
    }

    @Override
    public String toString() {
        return "Token{" +
                "type=" + type +
                ", lexeme='" + lexeme + '\'' +
                '}';
    }
}