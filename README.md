# Compiler Construction Project

## Overview
This project is a compiler for a custom programming language. It includes modules for lexical analysis, automata (DFA and NFA), and symbol table management.

## Modules and Classes

### Automata Module
- **DFA.java**: Represents a deterministic finite automaton.
  - Methods: `setStartState`, `addTransition`, `setFinalState`, `simulate`, `displayDFA`
- **DFABuilder.java**: Provides methods to create specific DFAs.
  - Methods: `createIdentifierDFA`, `createNumberDFA`, `createOperatorDFA`
- **NFA.java**: Represents a non-deterministic finite automaton.
  - Methods: `getStartState`, `getFinalStates`, `getStates`, `accepts`, `singleChar`, `concat`, `union`, `star`
- **RegexParser.java**: Converts regular expressions to NFAs.
  - Methods: `toNFA`
- **State.java**: Represents a state in an automaton.
  - Methods: `getId`, `isFinal`, `setFinal`, `addTransition`, `getTransitions`
- **Transition.java**: Represents a transition between states.
  - Methods: `getSymbol`, `getTo`
- **test.java**: Contains test cases for DFAs.
  - Methods: `main`, `test`

### Lexer Module
- **Lexer.java**: Tokenizes input code into a list of tokens.
  - Methods: `process`, `matchDFA`, `consumeSingleLineComment`, `consumeMultiLineComment`, `consumeString`, `displayTokens`
- **TestLexer.java**: Contains test cases for the lexer.
  - Methods: `main`
- **Token.java**: Represents a token in the input code.
  - Methods: `getType`, `getLexeme`, `toString`

### Symbols Module
- **SymbolTable.java**: Manages the symbol table for the compiler.
  - Methods: `insert`, `updateValue`, `lookup`, `remove`, `display`
- **SymbolTableEntry.java**: Represents an entry in the symbol table.
  - Methods: `getName`, `getDataType`, `getType`, `getScope`, `getValue`, `toString`

## Assignment Requirements
1. **RE, NFA, and DFA**:
   - Workflow for NFA, Regular Expression, and DFA classes.
   - Display total and unique states for each parse.
   - Show a transition state table.

2. **Lexical Analyzer**:
   - Tokenize source code into tokens.
   - Handle pre-processing and case sensitivity.
   - Display the number of tokens.

3. **Symbol Table**:
   - Track variables, functions, and identifiers.
   - Store details like names, types, and memory locations.
   - Handle entries for datatypes, input/output, strings, comments, constants, and arithmetic operations.

4. **Error Handler**:
   - Identify rule violations and show the line where the error occurs.

## Constraints
- Support Boolean, Integer, Decimal, and Character data types.
- Recognize lowercase letters as valid identifiers.
- Perform basic arithmetic operations.
- Handle decimal numbers up to five decimal places.
- Support exponents for whole and decimal numbers.
- Ignore extra spaces and handle multi-line comments.
- Support global and local variables.
- Display the transition state table.

## Usage Instructions
1. **Compile the project**: `javac -d bin src/**/*.java`
2. **Run the tests**: `java -cp bin automata.test`
3. **Run the lexer tests**: `java -cp bin lexer.TestLexer`

## Keywords and Rules
- **Keywords**: Define your own keywords.
- **Identifiers**: Only lowercase letters (a-z).
- **Numbers**: Integers and decimals up to 5 places.
- **Operators**: +, -, *, /, %, ^

## Example Code
```java
// Example code to be compiled by the custom compiler
int a = 5;
float b = 3.14;
boolean c = true;
char d = 'x';