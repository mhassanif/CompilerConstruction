#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

// Structure to represent the grammar with string-based symbols
struct Grammar {
    map<string, vector<vector<string>>> productions; // Non-terminal to list of productions
    string startSymbol; // Start symbol of the grammar
};

// Stack class for LL(1) parsing
struct Stack {
    vector<string> symbols; // Stack of symbols (terminals, non-terminals, $)

    // Push a single symbol onto the stack
    void push(const string& symbol) {
        symbols.push_back(symbol);
    }

    // Push a production's symbols in reverse order
    void push_production(const vector<string>& prod) {
        for (auto it = prod.rbegin(); it != prod.rend(); ++it) {
            push(*it);
        }
    }

    // Pop the top symbol and return it
    string pop() {
        string top = symbols.back();
        symbols.pop_back();
        return top;
    }

    // Get the top symbol without popping
    string top() const {
        return symbols.empty() ? "" : symbols.back();
    }

    // Check if the stack is empty
    bool empty() const {
        return symbols.empty();
    }

    // Get stack contents as a string for display
    string to_string() const {
        string result = "[";
        for (size_t i = 0; i < symbols.size(); ++i) {
            result += symbols[i];
            if (i < symbols.size() - 1) result += " ";
        }
        result += "]";
        return result;
    }
};

// Function to split a string by a delimiter
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

// Function to read grammar from file
Grammar readGrammar(const string& filename) {
    Grammar grammar;
    ifstream fin(filename);
    
    if (!fin) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    
    string line;
    bool isFirst = true;
    
    cout << "Original Grammar:" << endl;
    while (getline(fin, line)) {
        cout << line << endl;
        
        // Split line into non-terminal and productions (e.g., "S->int V ;|if ( E )")
        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos) continue;
        
        string nonTerminal = line.substr(0, arrowPos);
        nonTerminal.erase(remove_if(nonTerminal.begin(), nonTerminal.end(), ::isspace), nonTerminal.end());
        
        if (isFirst) {
            grammar.startSymbol = nonTerminal;
            isFirst = false;
        }
        
        // Split productions by '|'
        string prodStr = line.substr(arrowPos + 2);
        vector<string> prodParts = split(prodStr, '|');
        
        // Process each production
        for (const string& prod : prodParts) {
            vector<string> symbols = split(prod, ' ');
            grammar.productions[nonTerminal].push_back(symbols);
        }
    }
    
    fin.close();
    return grammar;
}

// Function to print the grammar
void printGrammar(const Grammar& grammar, const string& title) {
    cout << "\n" << title << ":" << endl;
    for (const auto& entry : grammar.productions) {
        cout << entry.first << " -> ";
        for (size_t i = 0; i < entry.second.size(); ++i) {
            for (const string& symbol : entry.second[i]) {
                cout << symbol << " ";
            }
            if (i < entry.second.size() - 1) cout << "| ";
        }
        cout << endl;
    }
}

// Function to find the common prefix of two productions
vector<string> findCommonPrefix(const vector<string>& prod1, const vector<string>& prod2) {
    vector<string> prefix;
    size_t minLen = min(prod1.size(), prod2.size());
    for (size_t i = 0; i < minLen && prod1[i] == prod2[i]; ++i) {
        prefix.push_back(prod1[i]);
    }
    return prefix;
}

// Left factoring function
Grammar applyLeftFactoring(const Grammar& original) {
    Grammar result = original;
    bool factored = false;
    map<string, vector<vector<string>>> newProductions;
    
    for (const auto& entry : result.productions) {
        string nonTerminal = entry.first;
        vector<vector<string>> productions = entry.second;
        
        // Check for common prefixes
        for (size_t i = 0; i < productions.size(); ++i) {
            for (size_t j = i + 1; j < productions.size(); ++j) {
                vector<string> prefix = findCommonPrefix(productions[i], productions[j]);
                
                if (!prefix.empty()) { // Common prefix found
                    factored = true;
                    
                    // Create a new non-terminal
                    string newNonTerminal = nonTerminal + "1";
                    while (result.productions.find(newNonTerminal) != result.productions.end() ||
                           newProductions.find(newNonTerminal) != newProductions.end()) {
                        newNonTerminal.back()++;
                    }
                    
                    // Create productions for the new non-terminal
                    vector<vector<string>> newNonTerminalProductions;
                    
                    // Add suffixes
                    vector<string> suffix1(productions[i].begin() + prefix.size(), productions[i].end());
                    vector<string> suffix2(productions[j].begin() + prefix.size(), productions[j].end());
                    
                    if (suffix1.empty()) suffix1 = {"e"};
                    if (suffix2.empty()) suffix2 = {"e"};
                    
                    newNonTerminalProductions.push_back(suffix1);
                    newNonTerminalProductions.push_back(suffix2);
                    
                    // Update original non-terminal's production
                    prefix.push_back(newNonTerminal);
                    
                    // Remove the two original productions
                    productions.erase(productions.begin() + j);
                    productions.erase(productions.begin() + i);
                    
                    // Add the new production
                    productions.push_back(prefix);
                    
                    // Store the new non-terminal
                    newProductions[newNonTerminal] = newNonTerminalProductions;
                    
                    // Restart checking
                    i = -1;
                    break;
                }
            }
        }
        
        result.productions[nonTerminal] = productions;
    }
    
    // Add new productions
    for (const auto& entry : newProductions) {
        result.productions[entry.first] = entry.second;
    }
    
    cout << "\n" << (factored ? "Left factoring was applied." : "No left factoring needed.") << endl;
    return result;
}

// DFS for FIRST set computation
bool calculateFirstDFS(const string& symbol, const string& original,
                      const map<string, vector<vector<string>>>& productions,
                      set<string>& firstSet) {
    // If terminal or epsilon
    if (productions.find(symbol) == productions.end() || symbol == "e") {
        firstSet.insert(symbol);
        return symbol == "e";
    }
    
    bool canDeriveEpsilon = false;
    
    for (const auto& production : productions.at(symbol)) {
        if (production.empty() || production == vector<string>{"e"}) {
            firstSet.insert("e");
            canDeriveEpsilon = true;
            continue;
        }
        
        bool allCanDeriveEpsilon = true;
        
        for (size_t i = 0; i < production.size(); ++i) {
            const string& currentSymbol = production[i];
            
            // Skip recursive non-terminal to avoid infinite loop
            if (currentSymbol == symbol) {
                allCanDeriveEpsilon = false;
                break;
            }
            
            // If terminal, add to FIRST and stop
            if (productions.find(currentSymbol) == productions.end()) {
                firstSet.insert(currentSymbol);
                allCanDeriveEpsilon = (currentSymbol == "e");
                break;
            }
            
            // Recursive call for non-terminal
            set<string> tempFirst;
            bool symbolCanDeriveEpsilon = calculateFirstDFS(currentSymbol, original,
                                                           productions, tempFirst);
            
            // Add terminals except epsilon
            for (const string& c : tempFirst) {
                if (c != "e") firstSet.insert(c);
            }
            
            if (!symbolCanDeriveEpsilon) {
                allCanDeriveEpsilon = false;
                break;
            }
            
            if (i == production.size() - 1 && symbolCanDeriveEpsilon) {
                canDeriveEpsilon = true;
            }
        }
        
        if (allCanDeriveEpsilon) {
            firstSet.insert("e");
            canDeriveEpsilon = true;
        }
    }
    
    return canDeriveEpsilon;
}

// Function to compute FIRST sets
map<string, set<string>> computeFirstSets(const Grammar& grammar) {
    map<string, set<string>> firstSets;
    
    for (const auto& entry : grammar.productions) {
        set<string> firstSet;
        calculateFirstDFS(entry.first, entry.first, grammar.productions, firstSet);
        firstSets[entry.first] = firstSet;
    }
    
    return firstSets;
}

// Function to compute FOLLOW sets
map<string, set<string>> computeFollowSets(const Grammar& grammar, const map<string, set<string>>& firstSets) {
    map<string, set<string>> followSets;
    
    // Initialize FOLLOW(start symbol) with $
    followSets[grammar.startSymbol].insert("$");
    
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& entry : grammar.productions) {
            string nonTerminal = entry.first;
            
            for (const auto& production : entry.second) {
                for (size_t i = 0; i < production.size(); ++i) {
                    if (grammar.productions.find(production[i]) != grammar.productions.end()) {
                        string B = production[i];
                        
                        // Case: B is followed by something
                        if (i + 1 < production.size()) {
                            string next = production[i + 1];
                            
                            // If next is a terminal
                            if (grammar.productions.find(next) == grammar.productions.end()) {
                                if (next != "e" && followSets[B].insert(next).second) {
                                    changed = true;
                                }
                            }
                            // If next is a non-terminal
                            else {
                                // Add FIRST(next) except epsilon
                                for (const string& c : firstSets.at(next)) {
                                    if (c != "e" && followSets[B].insert(c).second) {
                                        changed = true;
                                    }
                                }
                                
                                // If FIRST(next) contains epsilon
                                if (firstSets.at(next).find("e") != firstSets.at(next).end()) {
                                    bool allCanDeriveEpsilon = true;
                                    
                                    for (size_t j = i + 1; j < production.size(); ++j) {
                                        string symbol = production[j];
                                        
                                        if (grammar.productions.find(symbol) == grammar.productions.end()) {
                                            if (symbol != "e" && followSets[B].insert(symbol).second) {
                                                changed = true;
                                            }
                                            allCanDeriveEpsilon = (symbol == "e");
                                            break;
                                        }
                                        
                                        bool canDeriveEpsilon = false;
                                        for (const string& c : firstSets.at(symbol)) {
                                            if (c == "e") canDeriveEpsilon = true;
                                            else if (followSets[B].insert(c).second) {
                                                changed = true;
                                            }
                                        }
                                        
                                        if (!canDeriveEpsilon) {
                                            allCanDeriveEpsilon = false;
                                            break;
                                        }
                                    }
                                    
                                    if (allCanDeriveEpsilon) {
                                        for (const string& c : followSets[nonTerminal]) {
                                            if (followSets[B].insert(c).second) {
                                                changed = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // If B is the last symbol
                        else {
                            for (const string& c : followSets[nonTerminal]) {
                                if (followSets[B].insert(c).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return followSets;
}

// Function to remove left recursion
Grammar removeLeftRecursion(const Grammar& original) {
    Grammar result = original;
    bool hadLeftRecursion = false;
    
    vector<string> nonTerminals;
    for (const auto& entry : result.productions) {
        nonTerminals.push_back(entry.first);
    }
    sort(nonTerminals.begin(), nonTerminals.end());
    
    for (const string& Ai : nonTerminals) {
        // Handle indirect recursion
        for (const string& Aj : nonTerminals) {
            if (Aj >= Ai) break;
            
            vector<vector<string>> newProductions;
            
            for (const auto& production : result.productions[Ai]) {
                if (!production.empty() && production[0] == Aj) {
                    vector<string> gamma(production.begin() + 1, production.end());
                    for (const auto& delta : result.productions[Aj]) {
                        vector<string> newProd = delta;
                        newProd.insert(newProd.end(), gamma.begin(), gamma.end());
                        newProductions.push_back(newProd);
                    }
                } else {
                    newProductions.push_back(production);
                }
            }
            
            result.productions[Ai] = newProductions;
        }
        
        // Handle direct recursion
        vector<vector<string>> alphaProductions;
        vector<vector<string>> betaProductions;
        
        for (const auto& production : result.productions[Ai]) {
            if (!production.empty() && production[0] == Ai) {
                hadLeftRecursion = true;
                vector<string> alpha(production.begin() + 1, production.end());
                alphaProductions.push_back(alpha);
            } else {
                betaProductions.push_back(production);
            }
        }
        
        if (!alphaProductions.empty()) {
            string newNonTerminal = Ai + "1";
            while (result.productions.find(newNonTerminal) != result.productions.end()) {
                newNonTerminal.back()++;
            }
            
            vector<vector<string>> newAiProductions;
            vector<vector<string>> newAiPrimeProductions;
            
            for (const auto& beta : betaProductions) {
                vector<string> newProd = beta;
                if (beta == vector<string>{"e"}) {
                    newProd = {newNonTerminal};
                } else {
                    newProd.push_back(newNonTerminal);
                }
                newAiProductions.push_back(newProd);
            }
            
            if (betaProductions.empty()) {
                newAiProductions.push_back({newNonTerminal});
            }
            
            for (const auto& alpha : alphaProductions) {
                vector<string> newProd = alpha;
                newProd.push_back(newNonTerminal);
                newAiPrimeProductions.push_back(newProd);
            }
            
            newAiPrimeProductions.push_back({"e"});
            
            result.productions[Ai] = newAiProductions;
            result.productions[newNonTerminal] = newAiPrimeProductions;
            nonTerminals.push_back(newNonTerminal);
        }
    }
    
    cout << "\n" << (hadLeftRecursion ? "Left recursion was removed." : "No left recursion found.") << endl;
    return result;
}

// Function to print FIRST and FOLLOW sets
void printSets(const map<string, set<string>>& sets, const string& title) {
    cout << "\n" << title << ":" << endl;
    for (const auto& entry : sets) {
        cout << entry.first << " = {";
        bool first = true;
        for (const string& symbol : entry.second) {
            if (!first) cout << ", ";
            cout << symbol;
            first = false;
        }
        cout << "}" << endl;
    }
}

// Function to construct LL(1) parsing table
map<pair<string, string>, string> constructLL1Table(const Grammar& grammar,
                                                   const map<string, set<string>>& firstSets,
                                                   const map<string, set<string>>& followSets) {
    map<pair<string, string>, string> table;
    set<string> terminals;
    
    // Collect terminals
    for (const auto& entry : firstSets) {
        for (const string& terminal : entry.second) {
            if (terminal != "e") terminals.insert(terminal);
        }
    }
    for (const auto& entry : followSets) {
        for (const string& terminal : entry.second) {
            terminals.insert(terminal);
        }
    }
    
    // Build table
    for (const auto& entry : grammar.productions) {
        string nonTerminal = entry.first;
        
        for (size_t i = 0; i < entry.second.size(); ++i) {
            const vector<string>& production = entry.second[i];
            
            // Compute FIRST set of the production
            set<string> productionFirst;
            bool canDeriveEpsilon = true;
            
            for (const string& symbol : production) {
                if (symbol == "e") {
                    productionFirst.insert("e");
                    break;
                }
                
                if (grammar.productions.find(symbol) == grammar.productions.end()) {
                    productionFirst.insert(symbol);
                    canDeriveEpsilon = false;
                    break;
                }
                
                bool symbolCanDeriveEpsilon = false;
                for (const string& c : firstSets.at(symbol)) {
                    if (c == "e") symbolCanDeriveEpsilon = true;
                    else productionFirst.insert(c);
                }
                
                if (!symbolCanDeriveEpsilon) {
                    canDeriveEpsilon = false;
                    break;
                }
            }
            
            // If production derives epsilon, add FOLLOW terminals
            if (canDeriveEpsilon || production == vector<string>{"e"}) {
                productionFirst.insert("e");
                for (const string& c : followSets.at(nonTerminal)) {
                    productionFirst.insert(c);
                }
            }
            
            // Add production to table
            string prodStr = nonTerminal + " -> ";
            for (const string& s : production) prodStr += s + " ";
            
            for (const string& terminal : productionFirst) {
                if (terminal != "e") {
                    pair<string, string> key = {nonTerminal, terminal};
                    if (!table[key].empty()) {
                        table[key] += "/" + prodStr; // Conflict
                    } else {
                        table[key] = prodStr;
                    }
                }
            }
        }
    }
    
    // Print table
    cout << "\nLL(1) Parsing Table:" << endl;
    cout << setw(15) << " ";
    for (const string& terminal : terminals) {
        cout << setw(15) << terminal;
    }
    cout << endl;
    
    for (const auto& entry : grammar.productions) {
        string nonTerminal = entry.first;
        cout << setw(15) << nonTerminal;
        for (const string& terminal : terminals) {
            string entryStr = table[{nonTerminal, terminal}];
            cout << setw(15) << (entryStr.empty() ? "-" : entryStr);
        }
        cout << endl;
    }
    
    return table;
}

// Function to parse input strings
void parseInput(const string& filename, const Grammar& grammar,
                const map<string, set<string>>& firstSets,
                const map<string, set<string>>& followSets,
                const map<pair<string, string>, string>& parseTable) {
    ifstream fin(filename);
    if (!fin) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    
    cout << "\nParsing Input:\n" << endl;
    cout << setw(5) << "Step" << " | " << setw(20) << "Stack" << " | "
         << setw(15) << "Input" << " | " << setw(30) << "Action" << endl;
    cout << string(75, '-') << endl;
    
    string line;
    int lineNum = 0;
    int errorCount = 0;
    int step = 0;
    
    while (getline(fin, line)) {
        lineNum++;
        if (line.empty()) continue;
        
        // Split input line into tokens
        vector<string> tokens = split(line, ' ');
        tokens.push_back("$"); // Add end marker
        
        // Initialize stack
        Stack stack;
        stack.push("$");
        stack.push(grammar.startSymbol);
        
        size_t tokenIndex = 0;
        bool lineHasError = false;
        
        while (!stack.empty() && tokenIndex < tokens.size()) {
            string stackTop = stack.top();
            string currentToken = tokens[tokenIndex];
            
            // Print current state
            string action;
            if (stackTop == currentToken) {
                action = "Match " + currentToken;
                stack.pop();
                tokenIndex++;
            } else if (grammar.productions.find(stackTop) != grammar.productions.end()) {
                // Non-terminal: look up parse table
                string prod = parseTable.at({stackTop, currentToken});
                if (prod.empty()) {
                    // Error: no production
                    action = "Error: No production for " + stackTop + " on " + currentToken;
                    cout << setw(5) << step++ << " | " << setw(20) << stack.to_string()
                         << " | " << setw(15) << currentToken << " | " << setw(30) << action << endl;
                    
                    cout << "Line " << lineNum << ": Syntax Error: Unexpected token '"
                         << currentToken << "'\n";
                    errorCount++;
                    lineHasError = true;
                    
                    // Panic mode recovery: skip tokens until synchronization
                    while (tokenIndex < tokens.size() &&
                           followSets.at(stackTop).find(tokens[tokenIndex]) == followSets.at(stackTop).end()) {
                        tokenIndex++;
                    }
                    if (tokenIndex < tokens.size()) {
                        action = "Recover: Skip to " + tokens[tokenIndex];
                        cout << setw(5) << step++ << " | " << setw(20) << stack.to_string()
                             << " | " << setw(15) << tokens[tokenIndex] << " | " << setw(30) << action << endl;
                    }
                    stack.pop(); // Pop non-terminal
                    continue;
                }
                
                // Expand non-terminal
                stack.pop();
                for (const auto& prodVec : grammar.productions.at(stackTop)) {
                    string prodStr = stackTop + " -> ";
                    for (const string& s : prodVec) prodStr += s + " ";
                    if (prodStr == prod) {
                        stack.push_production(prodVec);
                        break;
                    }
                }
                action = "Expand " + prod;
            } else {
                // Error: mismatch
                action = "Error: Expected " + stackTop + ", got " + currentToken;
                cout << setw(5) << step++ << " | " << setw(20) << stack.to_string()
                     << " | " << setw(15) << currentToken << " | " << setw(30) << action << endl;
                
                cout << "Line " << lineNum << ": Syntax Error: Expected '" << stackTop
                     << "' but got '" << currentToken << "'\n";
                errorCount++;
                lineHasError = true;
                
                // Panic mode recovery: skip token or pop stack
                if (followSets.at(grammar.startSymbol).find(currentToken) != followSets.at(grammar.startSymbol).end()) {
                    stack.pop();
                    action = "Recover: Pop " + stackTop;
                } else {
                    tokenIndex++;
                    action = "Recover: Skip " + currentToken;
                }
                cout << setw(5) << step++ << " | " << setw(20) << stack.to_string()
                     << " | " << setw(15) << (tokenIndex < tokens.size() ? tokens[tokenIndex] : "")
                     << " | " << setw(30) << action << endl;
                continue;
            }
            
            cout << setw(5) << step++ << " | " << setw(20) << stack.to_string()
                 << " | " << setw(15) << currentToken << " | " << setw(30) << action << endl;
        }
        
        // Check if parsing completed successfully
        if (!lineHasError && stack.empty() && tokenIndex == tokens.size()) {
            cout << "Line " << lineNum << ": Parsed successfully.\n";
        } else if (!lineHasError) {
            cout << "Line " << lineNum << ": Syntax Error: Incomplete parse.\n";
            errorCount++;
        }
        
        if (lineHasError) {
            cout << "Line " << lineNum << ": Parsing continued after error recovery.\n";
        }
    }
    
    fin.close();
    
    // Print final message
    cout << "\nParsing completed " << (errorCount == 0 ? "successfully." : "with " + to_string(errorCount) + " errors.") << endl;
}

int main() {
    // Read grammar
    string grammarFile = "grammar.txt";
    Grammar originalGrammar = readGrammar(grammarFile);
    
    // Apply left factoring
    Grammar factoredGrammar = applyLeftFactoring(originalGrammar);
    printGrammar(factoredGrammar, "Grammar After Left Factoring");
    
    // Remove left recursion
    Grammar finalGrammar = removeLeftRecursion(factoredGrammar);
    printGrammar(finalGrammar, "Grammar After Left Recursion Removal");
    
    // Compute FIRST and FOLLOW sets
    map<string, set<string>> firstSets = computeFirstSets(finalGrammar);
    printSets(firstSets, "FIRST Sets");
    
    map<string, set<string>> followSets = computeFollowSets(finalGrammar, firstSets);
    printSets(followSets, "FOLLOW Sets");
    
    // Construct LL(1) parsing table
    map<pair<string, string>, string> parseTable = constructLL1Table(finalGrammar, firstSets, followSets);
    
    // Parse input strings
    string inputFile = "input.txt";
    parseInput(inputFile, finalGrammar, firstSets, followSets, parseTable);
    
    cout << "\nDone!" << endl;
    return 0;
}