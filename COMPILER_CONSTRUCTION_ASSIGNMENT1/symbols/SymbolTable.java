package symbols;

import java.util.*;

public class SymbolTable {
    private Map<String, SymbolTableEntry> table; // Store symbols

    public SymbolTable() 
    {
        this.table = new HashMap<>();
    }

    public void insert(String name, String dataType, SymbolTableEntry.SymbolType type, String scope, String value) 
    {
        if (table.containsKey(name)) 
        {
            SymbolTableEntry existingEntry = table.get(name);
            if (existingEntry.getType() == SymbolTableEntry.SymbolType.VARIABLE || existingEntry.getType() == SymbolTableEntry.SymbolType.CONSTANT) 
            {
                return; 
            }
            System.out.println("Error: Symbol '" + name + "' is already declared.");
            return;
        }
        table.put(name, new SymbolTableEntry(name, dataType, type, scope, value));
    }

    public void updateValue(String name, String value) 
    {
        if (table.containsKey(name)) 
        {
            SymbolTableEntry entry = table.get(name);
            table.put(name, new SymbolTableEntry(name, entry.getDataType(), entry.getType(), entry.getScope(), value));
        } 
        else
{
            System.out.println("Error: Symbol '" + name + "' not found.");
        }
    }

    public SymbolTableEntry lookup(String name) 
    {
        return table.get(name);
    }

    public void remove(String name)
    {
        table.remove(name);
    }

 // Display the symbol table in the required format
    public void display() {
        System.out.println("\n=== Symbol Table ===");
        System.out.println("ST { name : , type : , scope : , value : }"); 
        for (SymbolTableEntry entry : table.values())
        {
            System.out.println(entry); 
        }
    }

}