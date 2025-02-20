# Compiler Construction

## Team Details
- ### **Hassan Imran , 22I-0813 , Section-E**
- ### **Mohammad Bilal , 22I-0806 , Section-E**

---

## Project Overview
This project implements a **compiler for a custom programming language**. The compiler is built modularly, with components for **lexical analysis**, **automata (DFA and NFA)**, and **symbol table management**. It handles source code parsing, tokenization, and error detection for the defined language specifications.

---

## Modules and Classes

### **Automata Module**
This module handles **deterministic (DFA)** and **non-deterministic finite automata (NFA)** operations, including regular expression parsing.

- **DFA.java**  
  Represents a deterministic finite automaton.  
  **Methods**:  
    - `setStartState`  
    - `addTransition`  
    - `setFinalState`  
    - `simulate`  
    - `displayDFA`

- **DFABuilder.java**  
  Provides pre-defined DFAs for specific token types.  
  **Methods**:  
    - `createIdentifierDFA`  
    - `createNumberDFA`  
    - `createOperatorDFA`

- **NFA.java**  
  Represents a non-deterministic finite automaton.  
  **Methods**:  
    - `getStartState`  
    - `getFinalStates`  
    - `getStates`  
    - `accepts`  
    - `singleChar`  
    - `concat`  
    - `union`  
    - `star`

- **RegexParser.java**  
  Converts regular expressions into NFAs.  
  **Methods**: `toNFA`

- **State.java**  
  Represents a state in an automaton.  
  **Methods**:  
    - `getId`  
    - `isFinal`  
    - `setFinal`  
    - `addTransition`  
    - `getTransitions`

- **Transition.java**  
  Represents a transition between states.  
  **Methods**:  
    - `getSymbol`  
    - `getTo`

- **test.java**  
  Contains test cases for DFAs.  
  **Methods**:  
    - `main`  
    - `test`

---

### **Lexer Module**
This module tokenizes the input source code into recognizable tokens for further processing.

- **Lexer.java**  
  Core lexer logic for tokenization.  
  **Methods**:  
    - `process`  
    - `matchDFA`  
    - `consumeSingleLineComment`  
    - `consumeMultiLineComment`  
    - `consumeString`  
    - `displayTokens`

- **TestLexer.java**  
  Unit tests for the lexer module.  
  **Methods**: `main`

- **Token.java**  
  Represents a token in the source code.  
  **Methods**:  
    - `getType`  
    - `getLexeme`  
    - `toString`

---

### **Symbols Module**
This module manages the symbol table, tracking variables, functions, and identifiers with their attributes.

- **SymbolTable.java**  
  Manages entries in the symbol table.  
  **Methods**:  
    - `insert`  
    - `updateValue`  
    - `lookup`  
    - `remove`  
    - `display`

- **SymbolTableEntry.java**  
  Represents a single entry in the symbol table.  
  **Methods**:  
    - `getName`  
    - `getDataType`  
    - `getType`  
    - `getScope`  
    - `getValue`  
    - `toString`

---

## Assignment Requirements

### 1. Regular Expressions, NFA, and DFA
- Implement workflows for NFA and DFA generation from regular expressions.
- Display:
  - Total and unique states in the automata.
  - Transition state tables.

### 2. Lexical Analyzer
- Tokenize source code into tokens for keywords, identifiers, literals, and operators.
- Features:
  - Pre-processing and case insensitivity.
  - Multi-line and single-line comment handling.
  - Display the number and list of tokens.

### 3. Symbol Table
- Track and manage variables, functions, and identifiers.
- Store details such as:
  - Name, type, memory location, and scope.
- Handle:
  - Data types, input/output, strings, constants, arithmetic operations.

### 4. Error Handling
- Detect and report violations of language rules.
- Include the line number where errors occur.

---

## Language Specifications

### **Keywords**
Custom-defined keywords for the language.

### **Identifiers**
- Valid identifiers: lowercase letters (`a-z`).

### **Numbers**
- Support integers and decimals (up to 5 decimal places).
- Support scientific notation for numbers with exponents.

### **Operators**
Supported operators include:  
`+`, `-`, `*`, `/`, `%`, `^`.

### **Other Rules**
- Ignore extra spaces and handle multi-line comments.
- Support global and local variables.

---

## Example Code
Sample code that adheres to the language specifications:

```java
function myfunction() {
  wn x = 10;
  decimal y = x + 525;
  print(y);
}
```


