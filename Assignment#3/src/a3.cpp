#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stack>

using namespace std;

// Structure for Grammar. It keep production rules and starting symbol
struct Grammar {
    map<string, vector<vector<string>>> productions; // Production rules, like A -> B C | D
    string startSymbol; // This is where grammar start
};

// Define LL(1) parse table
typedef map<string, map<string, vector<string>>> ParseTable; // First map non-terminal, second map terminal, then production

// Class to handle parsing stack
class ParsingStack {
private:
    stack<string> st; // Stack hold the symbols

public:
    void push(const string& symbol) { // Push symbol to stack
        st.push(symbol);
    }

    string pop() { // Pop top element from stack
        if (st.empty()) { // If nothing inside, show error
            cerr << "Error: Stack underflow!" << endl;
            return "";
        }
        string top = st.top(); // Get top element
        st.pop(); // Remove top element
        return top;
    }

    string top() { // Just see top element, no remove
        if (st.empty()) { // If stack empty, show error
            cerr << "Error: Empty stack!" << endl;
            return "";
        }
        return st.top(); // Give top element
    }

    bool empty() { // Check if stack no have anything
        return st.empty();
    }

    string toString() { // Make stack elements into nice string
        string result = "[";
        stack<string> temp = st; // Copy of stack, so original not change
        vector<string> contents;

        while (!temp.empty()) { // Take all element one by one
            contents.push_back(temp.top());
            temp.pop();
        }

        reverse(contents.begin(), contents.end()); // Put back in correct order

        for (size_t i = 0; i < contents.size(); i++) { // Join with space
            result += contents[i];
            if (i < contents.size() - 1) {
                result += " ";
            }
        }

        result += "]";
        return result;
    }
};

// Function for reading grammar from file
Grammar readGrammar(const string& filename) {
    Grammar grammar;
    ifstream fin(filename); // Open the file

    if (!fin) { // If can't open, show problem
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    string line;
    bool isFirst = true; // First line have start symbol

    cout << "Original Grammar:" << endl;
    while (getline(fin, line)) { // Read line by line
        cout << line << endl;

        if (isFirst) { // First line, so set start symbol
            grammar.startSymbol = string(1, line[0]);
            isFirst = false;
        }

        string nonTerminal = string(1, line[0]); // First character is non-terminal
        vector<vector<string>> productions;
        vector<string> currentProduction;

        string rhs = line.substr(3); // Skip A-> part
        istringstream iss(rhs);
        string token;
        while (iss >> token) { // Split using spaces
            if (token == "|") { // New production if |
                productions.push_back(currentProduction);
                currentProduction.clear();
            } else {
                currentProduction.push_back(token); // Add token to current production
            }
        }
        if (!currentProduction.empty())
            productions.push_back(currentProduction);

        grammar.productions[nonTerminal] = productions; // Save to grammar
    }

    fin.close();
    return grammar; // Give grammar
}

// Function to print grammar nicely
void printGrammar(const Grammar& grammar, const string& title) {
    cout << "\n" << title << ":" << endl;
    for (const auto& entry : grammar.productions) { // For each non-terminal
        cout << entry.first << "->";
        for (size_t i = 0; i < entry.second.size(); i++) { // Each production
            for (size_t j = 0; j < entry.second[i].size(); j++) {
                cout << entry.second[i][j];
                if (j < entry.second[i].size() - 1) cout << " ";
            }
            if (i < entry.second.size() - 1) {
                cout << "|"; // Separate with |
            }
        }
        cout << endl;
    }
}

// Function to do left factoring
Grammar applyLeftFactoring(const Grammar& original) {
    Grammar result = original;
    bool factored = false; // Track if factoring done
    map<string, vector<vector<string>>> newProductions;

    for (const auto& entry : result.productions) {
        string nonTerminal = entry.first;
        vector<vector<string>> productions = entry.second;

        // Try find common prefix
        for (size_t i = 0; i < productions.size(); i++) {
            for (size_t j = i + 1; j < productions.size(); j++) {
                // Find how many same in start
                size_t k = 0;
                while (k < productions[i].size() && k < productions[j].size() &&
                       productions[i][k] == productions[j][k]) {
                    k++;
                }

                if (k > 0) {  // If common prefix found
                    factored = true;

                    // Make new non-terminal
                    string newNonTerminal = "Z";
                    while (result.productions.find(newNonTerminal) != result.productions.end() ||
                           newProductions.find(newNonTerminal) != newProductions.end()) {
                        newNonTerminal[0]--; // Try Z, then Y, X, etc
                    }

                    // Make production for new non-terminal
                    vector<vector<string>> newNonTerminalProductions;

                    // Take remaining part after prefix
                    vector<string> suffix1(productions[i].begin() + k, productions[i].end());
                    vector<string> suffix2(productions[j].begin() + k, productions[j].end());

                    if (suffix1.empty()) suffix1.push_back("e"); // If nothing left, use epsilon
                    if (suffix2.empty()) suffix2.push_back("e");

                    newNonTerminalProductions.push_back(suffix1);
                    newNonTerminalProductions.push_back(suffix2);

                    // Update original non-terminal
                    vector<string> prefix(productions[i].begin(), productions[i].begin() + k);
                    prefix.push_back(newNonTerminal);

                    // Remove old productions
                    if (j > i) {
                        productions.erase(productions.begin() + j);
                        productions.erase(productions.begin() + i);
                    } else {
                        productions.erase(productions.begin() + i);
                        productions.erase(productions.begin() + j);
                    }

                    productions.push_back(prefix); // Add new one with prefix

                    newProductions[newNonTerminal] = newNonTerminalProductions; // Save new non-terminal

                    i = -1; // Start again
                    break;
                }
            }
        }

        result.productions[nonTerminal] = productions; // Update
    }

    for (const auto& entry : newProductions) { // Add all new productions
        result.productions[entry.first] = entry.second;
    }

    if (factored) {
        cout << "\nLeft factoring was applied." << endl;
    } else {
        cout << "\nNo left factoring needed." << endl;
    }

    return result;
}

// DFS to find first set
bool calculateFirstDFS(const string& symbol, const string& original, const string& last,
                     const map<string, vector<vector<string>>>& productions,
                     set<string>& firstSet) {
    if (!(symbol[0] >= 'A' && symbol[0] <= 'Z')) { // If terminal or epsilon
        firstSet.insert(symbol);
        return symbol == "e";
    }

    bool canDeriveEpsilon = false; // Track if epsilon possible

    for (const auto& production : productions.at(symbol)) {
        if (production.empty()) {
            firstSet.insert("e"); // Empty means epsilon
            canDeriveEpsilon = true;
            continue;
        }

        bool allCanDeriveEpsilon = true; // Assume all symbols can epsilon

        for (size_t i = 0; i < production.size(); i++) {
            string currentSymbol = production[i];

            if (currentSymbol == symbol) { // Left recursion, skip
                allCanDeriveEpsilon = false;
                break;
            }

            if (!(currentSymbol[0] >= 'A' && currentSymbol[0] <= 'Z')) { // If terminal
                firstSet.insert(currentSymbol);
                allCanDeriveEpsilon = (currentSymbol == "e");
                break;
            }

            set<string> tempFirst;
            bool symbolCanDeriveEpsilon = calculateFirstDFS(currentSymbol, original, production.back(),
                                                         productions, tempFirst);

            for (const string& c : tempFirst) {
                if (c != "e") {
                    firstSet.insert(c); // Add terminals
                }
            }

            if (!symbolCanDeriveEpsilon) {
                allCanDeriveEpsilon = false;
                break;
            }

            if (i == production.size() - 1 && symbolCanDeriveEpsilon) {
                canDeriveEpsilon = true;
            }
        }

        if (allCanDeriveEpsilon) { // If whole production can epsilon
            firstSet.insert("e");
            canDeriveEpsilon = true;
        }
    }

    return canDeriveEpsilon;
}

// Function to compute first sets for all non-terminals
map<string, set<string>> computeFirstSets(const Grammar& grammar) {
    map<string, set<string>> firstSets;

    for (const auto& entry : grammar.productions) { // For every non-terminal
        string nonTerminal = entry.first;
        set<string> firstSet;
        calculateFirstDFS(nonTerminal, nonTerminal, nonTerminal, grammar.productions, firstSet);
        firstSets[nonTerminal] = firstSet; // Save first set
    }

    return firstSets;
}


// Function for find FOLLOW sets
map<string, set<string>> computeFollowSets(const Grammar& grammar, const map<string, set<string>>& firstSets) {
    map<string, set<string>> followSets; // we making map for FOLLOW set

    // For start symbol we put $ because it should end
    followSets[grammar.startSymbol].insert("$");

    // We keep doing until nothing change
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& entry : grammar.productions) { // look for every non-terminal
            string nonTerminal = entry.first;

            for (const auto& production : entry.second) { // look every rule of non-terminal
                for (size_t i = 0; i < production.size(); i++) {
                    // We only care if it non-terminal
                    if (production[i][0] >= 'A' && production[i][0] <= 'Z') {
                        string B = production[i]; // B is that non-terminal

                        // if after B there is something
                        if (i + 1 < production.size()) {
                            string next = production[i + 1];

                            // if next is terminal, add it in FOLLOW(B)
                            if (!(next[0] >= 'A' && next[0] <= 'Z')) {
                                if (next != "e" && followSets[B].insert(next).second) {
                                    changed = true; // something new added
                                }
                            }
                            // if next is also non-terminal
                            else {
                                // add FIRST(next) to FOLLOW(B) but not epsilon
                                for (const string& c : firstSets.at(next)) {
                                    if (c != "e" && followSets[B].insert(c).second) {
                                        changed = true;
                                    }
                                }

                                // if FIRST(next) got epsilon, check further
                                if (firstSets.at(next).find("e") != firstSets.at(next).end()) {
                                    bool allCanDeriveEpsilon = true;

                                    for (size_t j = i + 1; j < production.size(); j++) {
                                        string symbol = production[j];

                                        if (!(symbol[0] >= 'A' && symbol[0] <= 'Z')) { // if terminal
                                            if (symbol != "e") {
                                                if (followSets[B].insert(symbol).second) {
                                                    changed = true;
                                                }
                                            }
                                            allCanDeriveEpsilon = (symbol == "e");
                                            break;
                                        }

                                        // Add terminals from FIRST(symbol)
                                        bool canDeriveEpsilon = false;
                                        for (const string& c : firstSets.at(symbol)) {
                                            if (c == "e") {
                                                canDeriveEpsilon = true;
                                            } else if (followSets[B].insert(c).second) {
                                                changed = true;
                                            }
                                        }

                                        // if symbol cannot give epsilon, no need go further
                                        if (!canDeriveEpsilon) {
                                            allCanDeriveEpsilon = false;
                                            break;
                                        }
                                    }

                                    // if everything after can give epsilon, add FOLLOW(A) to FOLLOW(B)
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
                        // if B is last symbol
                        else {
                            // Add FOLLOW(A) to FOLLOW(B)
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

    return followSets; // return final FOLLOW sets
}

// Function for removing left recursion
Grammar removeLeftRecursion(const Grammar& original) {
    Grammar result = original;
    bool hadLeftRecursion = false; // flag for if we found left recursion

    // We store all non-terminals in a vector
    vector<string> nonTerminals;
    for (const auto& entry : result.productions) {
        nonTerminals.push_back(entry.first);
    }

    // Sort to make sure same order always
    sort(nonTerminals.begin(), nonTerminals.end());

    // Algorithm to remove left recursion
    for (const string& Ai : nonTerminals) {
        // Look at all non-terminals before Ai
        for (const string& Aj : nonTerminals) {
            if (Aj >= Ai) break;

            vector<vector<string>> newProductions;

            // Replace Ai -> Aj γ with Ai -> δ γ
            for (const auto& production : result.productions[Ai]) {
                if (!production.empty() && production[0] == Aj) {
                    // production start with Aj
                    vector<string> gamma(production.begin() + 1, production.end());

                    for (const auto& deltaProduction : result.productions[Aj]) {
                        vector<string> newProduction = deltaProduction;
                        newProduction.insert(newProduction.end(), gamma.begin(), gamma.end());
                        newProductions.push_back(newProduction);
                    }
                } else {
                    // keep same production
                    newProductions.push_back(production);
                }
            }

            result.productions[Ai] = newProductions;
        }

        // Now remove direct left recursion in Ai
        vector<vector<string>> alphaProductions;  // production like Ai -> Ai a
        vector<vector<string>> betaProductions;   // production like Ai -> β

        for (const auto& production : result.productions[Ai]) {
            if (!production.empty() && production[0] == Ai) {
                // this production have left recursion
                hadLeftRecursion = true;
                vector<string> alpha(production.begin() + 1, production.end());
                alphaProductions.push_back(alpha);
            } else {
                // no left recursion here
                betaProductions.push_back(production);
            }
        }

        if (!alphaProductions.empty()) {
            // Create new non-terminal like Ai'
            string newNonTerminal = Ai + "'";
            while (result.productions.find(newNonTerminal) != result.productions.end()) {
                newNonTerminal += "'";
            }

            // New rules: Ai -> β Ai' and Ai' -> a Ai' | e
            vector<vector<string>> newAiProductions;
            vector<vector<string>> newAiPrimeProductions;

            for (const auto& beta : betaProductions) {
                vector<string> newProduction = beta;
                if (beta.size() == 1 && beta[0] == "e") {
                    newProduction = {newNonTerminal};
                } else {
                    newProduction.push_back(newNonTerminal);
                }
                newAiProductions.push_back(newProduction);
            }

            // If no beta, add Ai -> Ai'
            if (betaProductions.empty()) {
                newAiProductions.push_back({newNonTerminal});
            }

            for (const auto& alpha : alphaProductions) {
                vector<string> newProduction = alpha;
                newProduction.push_back(newNonTerminal);
                newAiPrimeProductions.push_back(newProduction);
            }

            // add epsilon to Ai'
            newAiPrimeProductions.push_back({"e"});

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

// Function for printing FIRST and FOLLOW sets
void printSets(const map<string, set<string>>& sets, const string& title) {
    cout << "\n" << title << ":" << endl;
    for (const auto& entry : sets) {
        cout << entry.first << " = {";
        bool first = true;
        for (const string& symbol : entry.second) {
            if (!first) cout << ",";
            cout << symbol;
            first = false;
        }
        cout << "}" << endl;
    }
}


// Make LL(1) parsing table
ParseTable constructLL1Table(const Grammar& grammar,
    const map<string, set<string>>& firstSets,
    const map<string, set<string>>& followSets) {
cout << "\nConstructing LL(1) Parsing Table..." << endl;

ParseTable table;

// Find all terminal from grammar
set<string> terminals;
for (const auto& entry : firstSets) {
for (const string& terminal : entry.second) {
if (terminal != "e") {
terminals.insert(terminal);
}
}
}
for (const auto& entry : followSets) {
for (const string& terminal : entry.second) {
terminals.insert(terminal);
}
}

// For every non-terminal
for (const auto& entry : grammar.productions) {
string nonTerminal = entry.first;

// See all production rules
for (size_t i = 0; i < entry.second.size(); i++) {
const vector<string>& production = entry.second[i];

// Get FIRST set for production
set<string> productionFirst;
bool canDeriveEpsilon = true;

for (const string& symbol : production) {
if (symbol == "e") {
productionFirst.insert("e");
break;
}

if (!(symbol[0] >= 'A' && symbol[0] <= 'Z')) {
productionFirst.insert(symbol);
canDeriveEpsilon = false;
break;
}

bool symbolCanDeriveEpsilon = false;
for (const string& c : firstSets.at(symbol)) {
if (c == "e") {
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

// If production can go to epsilon, add FOLLOW set too
if (canDeriveEpsilon || (production.size() == 1 && production[0] == "e")) {
productionFirst.insert("e");
for (const string& c : followSets.at(nonTerminal)) {
productionFirst.insert(c);
}
}

// Now fill the table
for (const string& terminal : terminals) {
if (productionFirst.find(terminal) != productionFirst.end()) {
if (terminal != "e") {
   // Conflict happen maybe
   if (!table[nonTerminal][terminal].empty()) {
       cout << "Warning: Grammar not LL(1)! Conflict at [" << nonTerminal << ", "
            << terminal << "]" << endl;
   }

   table[nonTerminal][terminal] = production;
}
}
}

// Handle if production is epsilon
if (productionFirst.find("e") != productionFirst.end()) {
for (const string& c : followSets.at(nonTerminal)) {
if (!table[nonTerminal][c].empty()) {
   cout << "Warning: Grammar not LL(1)! Conflict at [" << nonTerminal << ", "
        << c << "]" << endl;
}

// For epsilon, put it in table for FOLLOW symbols
if (production.size() == 1 && production[0] == "e") {
   table[nonTerminal][c] = {"e"};
} else {
   table[nonTerminal][c] = production;
}
}
}
}
}

// Show the parsing table
cout << "\nLL(1) Parsing Table:" << endl;
cout << setw(15) << " ";
for (const string& terminal : terminals) {
cout << setw(15) << terminal;
}
cout << endl;

for (const auto& row : table) {
cout << setw(15) << row.first;
for (const string& terminal : terminals) {
string entry;
if (row.second.find(terminal) != row.second.end()) {
entry = row.first + "->";
for (size_t i = 0; i < row.second.at(terminal).size(); ++i) {
entry += row.second.at(terminal)[i];
if (i < row.second.at(terminal).size() - 1) entry += " ";
}
}
cout << setw(15) << entry;
}
cout << endl;
}

return table;
}

// Parse one line input
void parseInput(const ParseTable& table, const Grammar& grammar, const string& input) {
cout << "\nParsing input: " << input << endl;
cout << string(50, '-') << endl;
cout << setw(20) << "Stack" << setw(20) << "Input" << setw(20) << "Action" << endl;
cout << string(50, '-') << endl;

ParsingStack stack;
stack.push("$");  // Put end marker
stack.push(grammar.startSymbol);  // Put start symbol

istringstream iss(input);
vector<string> inputSymbols;
string symbol;

// Break input into symbols
while (iss >> symbol) {
inputSymbols.push_back(symbol);
}
inputSymbols.push_back("$");  // Put end marker

size_t inputPos = 0;
int errorCount = 0;

while (!stack.empty()) {
// Show current stack and input
string currentInput;
for (size_t i = inputPos; i < inputSymbols.size(); i++) {
currentInput += inputSymbols[i];
currentInput += ' ';
}

cout << setw(20) << stack.toString() << setw(20) << currentInput;

// If input finish but stack not empty
if (inputPos >= inputSymbols.size()) {
cout << setw(20) << "Error: Unexpected end of input" << endl;
errorCount++;
break;
}

string currentInputSymbol = inputSymbols[inputPos];
string topStack = stack.top();

// Both stack and input at $, success
if (topStack == "$" && currentInputSymbol == "$") {
cout << setw(20) << "Accept" << endl;
break;
}

// Stack top is terminal, match with input
if (!(topStack[0] >= 'A' && topStack[0] <= 'Z')) {
if (topStack == currentInputSymbol) {
cout << setw(20) << "Match: " + topStack << endl;
stack.pop();
inputPos++;
} else {
cout << setw(20) << "Error: Expected " + topStack + " but found " + currentInputSymbol << endl;
errorCount++;

// Try fix by skip input symbol
inputPos++;
}
}
// Stack top is non-terminal, expand
else {
if (table.find(topStack) != table.end() &&
table.at(topStack).find(currentInputSymbol) != table.at(topStack).end()) {

vector<string> production = table.at(topStack).at(currentInputSymbol);
stack.pop();

string action = "Expand: " + topStack + " -> ";
for (const string& c : production) {
action += c + " ";
}
cout << setw(20) << action << endl;

// Push production in reverse, so pop correct later
if (!(production.size() == 1 && production[0] == "e")) {
for (auto it = production.rbegin(); it != production.rend(); ++it) {
   stack.push(*it);
}
}
// If epsilon, just pop
} else {
cout << setw(20) << "Error: No production for [" + topStack + ", " + currentInputSymbol + "]" << endl;
errorCount++;

// Try fix by pop from stack
stack.pop();
}
}
}

cout << string(50, '-') << endl;
if (errorCount > 0) {
cout << "Parsing done but " << errorCount << " error(s) happen." << endl;
} else {
cout << "Parsing done, no error!" << endl;
}
}

// Parse whole input file
void parseInputFile(const ParseTable& table, const Grammar& grammar, const string& filename) {
ifstream fin(filename);
if (!fin) {
cerr << "Error open input file: " << filename << endl;
exit(1);
}

string line;
int lineNum = 1;
int totalErrors = 0;

cout << "\nParsing input file: " << filename << endl;

while (getline(fin, line)) {
if (!line.empty()) {
cout << "\nLine " << lineNum << ": " << line << endl;
parseInput(table, grammar, line);
}
lineNum++;
}

cout << "\nParsing finished for all lines." << endl;
fin.close();
}

int main() {
// Read grammar from file
string grammarFile = "grammar.txt";
Grammar originalGrammar = readGrammar(grammarFile);

// Do left factoring
Grammar factoredGrammar = applyLeftFactoring(originalGrammar);
printGrammar(factoredGrammar, "Grammar After Left Factoring");

// Remove left recursion
Grammar finalGrammar = removeLeftRecursion(factoredGrammar);
printGrammar(finalGrammar, "Grammar After Left Recursion Removal");

// Get FIRST sets
map<string, set<string>> firstSets = computeFirstSets(finalGrammar);
printSets(firstSets, "FIRST Sets");

// Get FOLLOW sets
map<string, set<string>> followSets = computeFollowSets(finalGrammar, firstSets);
printSets(followSets, "FOLLOW Sets");

// Make parsing table
ParseTable parseTable = constructLL1Table(finalGrammar, firstSets, followSets);

// Parse input file
string inputFile = "input.txt";
parseInputFile(parseTable, finalGrammar, inputFile);

return 0;
}
