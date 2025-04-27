# Compiler Construction

## Team Details
- ### **Hassan Imran , 22I-0813 , Section-E**
- ### **Mohammad Bilal , 22I-0806 , Section-E**

---

## Project Overview
This project implements a **compiler for a custom programming language**. The compiler is built modularly, with components for **lexical analysis**, **automata (DFA and NFA)**, and **symbol table management**. It handles source code parsing, tokenization, and error detection for the defined language specifications.

## Token Rules and Keywords for Lexer

**1. Keywords** 
wn, decimal, var, if, else, while, print, input, return, true, false, function

**2. Operators**
=, +, -, *, /, %, &&, ||, !, ==, !=, <, >, <=, >=

**3. Separators**
(  )  {  }  ;  ,

**4. Token Matching Rules**
The lexer uses regular expressions to match different token types:

Identifiers: Must start with a letter (a-z)  and can be followed by letters
Numbers: Supports integer and floating-point numbers.
String Literals: Strings are enclosed in double quotes "...".

Comments:

Single-line comments: Start with $$ and extend to the end of the line.

Multi-line comments: Enclosed between #$ and $#.




**5. Token Types**

**Token Type**                                              **Description**

KEYWORD                                                    Reserved words in the language

IDENTIFIER                                                 Variable and function names

NUMBER                                                     Integer and floating-point numbers

OPERATOR                                                   Arithmetic and logical operators

SEPARATOR                                                  Punctuation marks and brackets

STRING                                                     Text enclosed in double quotes

COMMENT                                                    Single-line and multi-line comments

CONSTANT                                                   Numeric constants

FUNCTION                                                   Function names

INVALID                                                    Unrecognized tokens




