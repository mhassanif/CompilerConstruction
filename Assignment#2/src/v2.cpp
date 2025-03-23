#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

// Grammar representation
struct Grammar {
    map<char, vector<vector<char>>> productions;
    char startSymbol;
};

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
        
        if (isFirst) {
            grammar.startSymbol = line[0];
            isFirst = false;
        }
        
        char nonTerminal = line[0];
        vector<char> currentProduction;
        
        for (size_t i = 3; i < line.size(); i++) {
            if (line[i] == '|') {
                grammar.productions[nonTerminal].push_back(currentProduction);
                currentProduction.clear();
            } else {
                currentProduction.push_back(line[i]);
            }
        }
        
        // Add the last production
        grammar.productions[nonTerminal].push_back(currentProduction);
    }
    
    fin.close();
    return grammar;
}

// Function to print the grammar
void printGrammar(const Grammar& grammar, const string& title) {
    cout << "\n" << title << ":" << endl;
    for (const auto& entry : grammar.productions) {
        cout << entry.first << "->";
        for (size_t i = 0; i < entry.second.size(); i++) {
            for (char symbol : entry.second[i]) {
                cout << symbol;
            }
            if (i < entry.second.size() - 1) {
                cout << "|";
            }
        }
        cout << endl;
    }
}

// Left factoring function
Grammar applyLeftFactoring(const Grammar& original) {
    Grammar result = original;
    bool factored = false;
    map<char, vector<vector<char>>> newProductions;
    
    for (const auto& entry : result.productions) {
        char nonTerminal = entry.first;
        vector<vector<char>> productions = entry.second;
        
        // Check for common prefixes
        for (size_t i = 0; i < productions.size(); i++) {
            for (size_t j = i + 1; j < productions.size(); j++) {
                // Find common prefix
                size_t k = 0;
                while (k < productions[i].size() && k < productions[j].size() && 
                       productions[i][k] == productions[j][k]) {
                    k++;
                }
                
                if (k > 0) {  // Common prefix found
                    factored = true;
                    
                    // Create a new non-terminal
                    char newNonTerminal = 'Z';
                    while (result.productions.find(newNonTerminal) != result.productions.end() ||
                           newProductions.find(newNonTerminal) != newProductions.end()) {
                        newNonTerminal--;
                    }
                    
                    // Create production for the new non-terminal
                    vector<vector<char>> newNonTerminalProductions;
                    
                    // Add the remaining parts of both productions
                    vector<char> suffix1(productions[i].begin() + k, productions[i].end());
                    vector<char> suffix2(productions[j].begin() + k, productions[j].end());
                    
                    // If a suffix is empty, make it epsilon
                    if (suffix1.empty()) suffix1.push_back('e');
                    if (suffix2.empty()) suffix2.push_back('e');
                    
                    newNonTerminalProductions.push_back(suffix1);
                    newNonTerminalProductions.push_back(suffix2);
                    
                    // Update the original non-terminal's production
                    vector<char> prefix(productions[i].begin(), productions[i].begin() + k);
                    prefix.push_back(newNonTerminal);
                    
                    // Remove the two original productions
                    productions.erase(productions.begin() + j);
                    productions.erase(productions.begin() + i);
                    
                    // Add the new production
                    productions.push_back(prefix);
                    
                    // Store the new non-terminal and its productions
                    newProductions[newNonTerminal] = newNonTerminalProductions;
                    
                    // Reset to check all pairs again
                    i = -1;
                    break;
                }
            }
        }
        
        result.productions[nonTerminal] = productions;
    }
    
    // Add all new productions to the result
    for (const auto& entry : newProductions) {
        result.productions[entry.first] = entry.second;
    }
    
    if (factored) {
        cout << "\nLeft factoring was applied." << endl;
    } else {
        cout << "\nNo left factoring needed." << endl;
    }
    
    return result;
}

// Helper function for FIRST set calculation
bool calculateFirstDFS(char symbol, char original, char last, 
                     const map<char, vector<vector<char>>>& productions,
                     set<char>& firstSet) {
    if (!(symbol >= 'A' && symbol <= 'Z')) {
        // If terminal or epsilon
        firstSet.insert(symbol);
        return symbol == 'e';
    }
    
    bool canDeriveEpsilon = false;
    
    for (const auto& production : productions.at(symbol)) {
        if (production.empty()) {
            firstSet.insert('e');
            canDeriveEpsilon = true;
            continue;
        }
        
        // Check if this production can derive epsilon
        bool allCanDeriveEpsilon = true;
        
        for (size_t i = 0; i < production.size(); i++) {
            char currentSymbol = production[i];
            
            // If it's the same non-terminal (left recursion), skip to avoid infinite loop
            if (currentSymbol == symbol) {
                allCanDeriveEpsilon = false;
                break;
            }
            
            // If terminal, add to FIRST set and stop
            if (!(currentSymbol >= 'A' && currentSymbol <= 'Z')) {
                firstSet.insert(currentSymbol);
                allCanDeriveEpsilon = (currentSymbol == 'e');
                break;
            }
            
            // Recursive call for non-terminal
            set<char> tempFirst;
            bool symbolCanDeriveEpsilon = calculateFirstDFS(currentSymbol, original, production.back(), 
                                                         productions, tempFirst);
            
            // Add all terminals except epsilon to the FIRST set
            for (char c : tempFirst) {
                if (c != 'e') {
                    firstSet.insert(c);
                }
            }
            
            // If this symbol can't derive epsilon, we stop here
            if (!symbolCanDeriveEpsilon) {
                allCanDeriveEpsilon = false;
                break;
            }
            
            // If we're at the last symbol and it can derive epsilon, the whole production can derive epsilon
            if (i == production.size() - 1 && symbolCanDeriveEpsilon) {
                canDeriveEpsilon = true;
            }
        }
        
        // If all symbols in this production can derive epsilon, the whole non-terminal can
        if (allCanDeriveEpsilon) {
            firstSet.insert('e');
            canDeriveEpsilon = true;
        }
    }
    
    return canDeriveEpsilon;
}

// Function to compute FIRST sets
map<char, set<char>> computeFirstSets(const Grammar& grammar) {
    map<char, set<char>> firstSets;
    
    // For each non-terminal
    for (const auto& entry : grammar.productions) {
        char nonTerminal = entry.first;
        set<char> firstSet;
        calculateFirstDFS(nonTerminal, nonTerminal, nonTerminal, grammar.productions, firstSet);
        firstSets[nonTerminal] = firstSet;
    }
    
    return firstSets;
}

// Function to compute FOLLOW sets
map<char, set<char>> computeFollowSets(const Grammar& grammar, const map<char, set<char>>& firstSets) {
    map<char, set<char>> followSets;
    
    // Initialize FOLLOW set of start symbol with $
    followSets[grammar.startSymbol].insert('$');
    
    // Repeat until no more changes
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& entry : grammar.productions) {
            char nonTerminal = entry.first;
            
            for (const auto& production : entry.second) {
                for (size_t i = 0; i < production.size(); i++) {
                    // Only interested in non-terminals in the production
                    if (production[i] >= 'A' && production[i] <= 'Z') {
                        char B = production[i];
                        
                        // Case where B is followed by something
                        if (i + 1 < production.size()) {
                            char next = production[i + 1];
                            
                            // If next is a terminal, add it to FOLLOW(B)
                            if (!(next >= 'A' && next <= 'Z')) {
                                if (next != 'e' && followSets[B].insert(next).second) {
                                    changed = true;
                                }
                            }
                            // If next is a non-terminal
                            else {
                                // Add FIRST(next) except epsilon to FOLLOW(B)
                                for (char c : firstSets.at(next)) {
                                    if (c != 'e' && followSets[B].insert(c).second) {
                                        changed = true;
                                    }
                                }
                                
                                // If FIRST(next) contains epsilon, we need to look further
                                if (firstSets.at(next).find('e') != firstSets.at(next).end()) {
                                    bool allCanDeriveEpsilon = true;
                                    
                                    for (size_t j = i + 1; j < production.size(); j++) {
                                        char symbol = production[j];
                                        
                                        if (!(symbol >= 'A' && symbol <= 'Z')) {
                                            if (symbol != 'e') {
                                                if (followSets[B].insert(symbol).second) {
                                                    changed = true;
                                                }
                                            }
                                            allCanDeriveEpsilon = (symbol == 'e');
                                            break;
                                        }
                                        
                                        // Add all terminals from FIRST(symbol) to FOLLOW(B)
                                        bool canDeriveEpsilon = false;
                                        for (char c : firstSets.at(symbol)) {
                                            if (c == 'e') {
                                                canDeriveEpsilon = true;
                                            } else if (followSets[B].insert(c).second) {
                                                changed = true;
                                            }
                                        }
                                        
                                        if (!canDeriveEpsilon) {
                                            allCanDeriveEpsilon = false;
                                            break;
                                        }
                                    }
                                    
                                    // If all following symbols derive epsilon, add FOLLOW(A) to FOLLOW(B)
                                    if (allCanDeriveEpsilon) {
                                        for (char c : followSets[nonTerminal]) {
                                            if (followSets[B].insert(c).second) {
                                                changed = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // If B is the last symbol in the production
                        else {
                            // Add FOLLOW(A) to FOLLOW(B)
                            for (char c : followSets[nonTerminal]) {
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

// Function to detect and remove left recursion
Grammar removeLeftRecursion(const Grammar& original) {
    Grammar result = original;
    bool hadLeftRecursion = false;
    
    // Create a vector of non-terminals in order
    vector<char> nonTerminals;
    for (const auto& entry : result.productions) {
        nonTerminals.push_back(entry.first);
    }
    
    // Sort non-terminals to ensure deterministic ordering
    sort(nonTerminals.begin(), nonTerminals.end());
    
    // Apply the algorithm for removing left recursion
    for (char Ai : nonTerminals) {
        // For each earlier non-terminal Aj
        for (char Aj : nonTerminals) {
            if (Aj >= Ai) break;
            
            vector<vector<char>> newProductions;
            
            // Replace Ai -> Aj γ with Ai -> δ1 γ | δ2 γ | ... | δn γ
            // where Aj -> δ1 | δ2 | ... | δn
            for (const auto& production : result.productions[Ai]) {
                if (!production.empty() && production[0] == Aj) {
                    // This production starts with Aj
                    vector<char> gamma(production.begin() + 1, production.end());
                    
                    // For each production Aj -> δ
                    for (const auto& deltaProduction : result.productions[Aj]) {
                        vector<char> newProduction = deltaProduction;
                        newProduction.insert(newProduction.end(), gamma.begin(), gamma.end());
                        newProductions.push_back(newProduction);
                    }
                } else {
                    // Keep this production unchanged
                    newProductions.push_back(production);
                }
            }
            
            result.productions[Ai] = newProductions;
        }
        
        // Eliminate direct left recursion for Ai
        vector<vector<char>> alphaProductions;  // Ai -> Ai α
        vector<vector<char>> betaProductions;   // Ai -> β
        
        for (const auto& production : result.productions[Ai]) {
            if (!production.empty() && production[0] == Ai) {
                // This is a left-recursive production
                hadLeftRecursion = true;
                vector<char> alpha(production.begin() + 1, production.end());
                alphaProductions.push_back(alpha);
            } else {
                // This is not a left-recursive production
                betaProductions.push_back(production);
            }
        }
        
        if (!alphaProductions.empty()) {
            // Create a new non-terminal Ai'
            char newNonTerminal = Ai;
            newNonTerminal = 'A' + nonTerminals.size();
            while (result.productions.find(newNonTerminal) != result.productions.end()) {
                newNonTerminal++;
            }
            
            // Replace Ai -> Ai α | β with
            // Ai -> β Ai'
            // Ai' -> α Ai' | ε
            vector<vector<char>> newAiProductions;
            vector<vector<char>> newAiPrimeProductions;
            
            for (const auto& beta : betaProductions) {
                vector<char> newProduction = beta;
                if (beta.size() == 1 && beta[0] == 'e') {
                    // If β is ε, replace with Ai'
                    newProduction = {newNonTerminal};
                } else {
                    // Otherwise, append Ai' to β
                    newProduction.push_back(newNonTerminal);
                }
                newAiProductions.push_back(newProduction);
            }
            
            // If there are no β productions, add Ai -> Ai'
            if (betaProductions.empty()) {
                newAiProductions.push_back({newNonTerminal});
            }
            
            for (const auto& alpha : alphaProductions) {
                vector<char> newProduction = alpha;
                newProduction.push_back(newNonTerminal);
                newAiPrimeProductions.push_back(newProduction);
            }
            
            // Add ε production for Ai'
            newAiPrimeProductions.push_back({'e'});
            
            // Update the grammar
            result.productions[Ai] = newAiProductions;
            result.productions[newNonTerminal] = newAiPrimeProductions;
            nonTerminals.push_back(newNonTerminal);
        }
    }
    
    if (hadLeftRecursion) {
        cout << "\nLeft recursion was removed." << endl;
    } else {
        cout << "\nNo left recursion found." << endl;
    }
    
    return result;
}

// Function to print FIRST and FOLLOW sets
void printSets(const map<char, set<char>>& sets, const string& title) {
    cout << "\n" << title << ":" << endl;
    for (const auto& entry : sets) {
        cout << entry.first << " = {";
        bool first = true;
        for (char symbol : entry.second) {
            if (!first) cout << ",";
            cout << symbol;
            first = false;
        }
        cout << "}" << endl;
    }
}

// Function to construct LL(1) parsing table
void constructLL1Table(const Grammar& grammar, 
                       const map<char, set<char>>& firstSets, 
                       const map<char, set<char>>& followSets) {
    cout << "\nLL(1) Parsing Table:" << endl;
    
    // Find all terminals in the grammar
    set<char> terminals;
    for (const auto& entry : firstSets) {
        for (char terminal : entry.second) {
            if (terminal != 'e') {
                terminals.insert(terminal);
            }
        }
    }
    for (const auto& entry : followSets) {
        for (char terminal : entry.second) {
            terminals.insert(terminal);
        }
    }
    
    // Create and display the table header
    cout << setw(10) << " ";
    for (char terminal : terminals) {
        cout << setw(10) << terminal;
    }
    cout << endl;
    
    // For each non-terminal
    for (const auto& entry : grammar.productions) {
        char nonTerminal = entry.first;
        cout << setw(10) << nonTerminal;
        
        // For each terminal
        for (char terminal : terminals) {
            string tableEntry = "";
            
            // Check each production
            for (size_t i = 0; i < entry.second.size(); i++) {
                const vector<char>& production = entry.second[i];
                
                // Calculate FIRST set of the production
                set<char> productionFirst;
                bool canDeriveEpsilon = true;
                
                for (char symbol : production) {
                    if (symbol == 'e') {
                        productionFirst.insert('e');
                        break;
                    }
                    
                    if (!(symbol >= 'A' && symbol <= 'Z')) {
                        productionFirst.insert(symbol);
                        canDeriveEpsilon = false;
                        break;
                    }
                    
                    bool symbolCanDeriveEpsilon = false;
                    for (char c : firstSets.at(symbol)) {
                        if (c == 'e') {
                            symbolCanDeriveEpsilon = true;
                        } else {
                            productionFirst.insert(c);
                        }
                    }
                    
                    if (!symbolCanDeriveEpsilon) {
                        canDeriveEpsilon = false;
                        break;
                    }
                }
                
                // If the production can derive epsilon, add FOLLOW(A) to the FIRST set
                if (canDeriveEpsilon || (production.size() == 1 && production[0] == 'e')) {
                    productionFirst.insert('e');
                    for (char c : followSets.at(nonTerminal)) {
                        productionFirst.insert(c);
                    }
                }
                
                // If the terminal is in the FIRST set of the production, add the production to the table
                if (productionFirst.find(terminal) != productionFirst.end()) {
                    if (!tableEntry.empty()) {
                        tableEntry += "/";  // Conflict
                    }
                    
                    tableEntry += nonTerminal;
                    tableEntry += "->";
                    for (char symbol : production) {
                        tableEntry += symbol;
                    }
                }
            }
            
            cout << setw(10) << tableEntry;
        }
        
        cout << endl;
    }
}

int main() {
    // Step 1: Read grammar from file
    string filename = "example3.txt";
    
    Grammar originalGrammar = readGrammar(filename);
    
    // Step 2: Apply left factoring
    Grammar factoredGrammar = applyLeftFactoring(originalGrammar);
    printGrammar(factoredGrammar, "Grammar After Left Factoring");
    
    // Step 3: Remove left recursion
    Grammar finalGrammar = removeLeftRecursion(factoredGrammar);
    printGrammar(finalGrammar, "Grammar After Left Recursion Removal");
    
    // Step 4: Compute FIRST sets
    map<char, set<char>> firstSets = computeFirstSets(finalGrammar);
    printSets(firstSets, "FIRST Sets");
    
    // Step 5: Compute FOLLOW sets
    map<char, set<char>> followSets = computeFollowSets(finalGrammar, firstSets);
    printSets(followSets, "FOLLOW Sets");
    
    // Step 6: Construct LL(1) parsing table
    constructLL1Table(finalGrammar, firstSets, followSets);
    
    cout << "\nProgram completed successfully!" << endl;
    return 0;
}