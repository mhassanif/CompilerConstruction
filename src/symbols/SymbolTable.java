package symbols;

import java.util.*;

public class SymbolTable {
    private Map<String, SymbolTableEntry> table; // Store symbols

    public SymbolTable() {
        this.table = new HashMap<>();
    }

    //  Insert a symbol
    public void insert(String name, String dataType, SymbolTableEntry.SymbolType type, String scope, String value) {
        if (table.containsKey(name)) {
            SymbolTableEntry existingEntry = table.get(name);
            if (existingEntry.getType() == SymbolTableEntry.SymbolType.VARIABLE || existingEntry.getType() == SymbolTableEntry.SymbolType.CONSTANT) {
                return; //  Allow redeclaration if type matches
            }
            System.out.println("Error: Symbol '" + name + "' is already declared.");
            return;
        }
        table.put(name, new SymbolTableEntry(name, dataType, type, scope, value));
    }

    //  Update the value of an existing symbol
    public void updateValue(String name, String value) {
        if (table.containsKey(name)) {
            SymbolTableEntry entry = table.get(name);
            table.put(name, new SymbolTableEntry(name, entry.getDataType(), entry.getType(), entry.getScope(), value));
        } else {
            System.out.println("Error: Symbol '" + name + "' not found.");
        }
    }

    //  Lookup a symbol
    public SymbolTableEntry lookup(String name) {
        return table.get(name);
    }

    //  Remove a symbol
    public void remove(String name) {
        table.remove(name);
    }

    //  Display the symbol table
    public void display() {
        System.out.println("\n=== Symbol Table ===");
        for (SymbolTableEntry entry : table.values()) {
            System.out.println(entry);
        }
    }
}
