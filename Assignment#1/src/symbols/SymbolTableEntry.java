package symbols;

public class SymbolTableEntry {
    public enum SymbolType { VARIABLE, FUNCTION, CONSTANT, KEYWORD }

    private String name;
    private String dataType; // int, float, bool, string
    private SymbolType type;
    private String scope; 
    private String value; 
    public SymbolTableEntry(String name, String dataType, SymbolType type, String scope, String value) {
        this.name = name;
        this.dataType = dataType;
        this.type = type;
        this.scope = scope;
        this.value = value;
    }

  
    public String getName() 
    {
    	return name;
    	}
    public String getDataType() 
    {
    	return dataType; }
    public SymbolType getType() 
    { 
    	return type;
    }
    public String getScope() 
    { 
    	return scope; 
    	}
    public String getValue() 
    { 
    	return value;
}

    @Override
    public String toString() 
    {
        return "ST { name : " + name + " , type : " + type + " , scope : " + scope + " , value : " + (value != null ? value : "null") + " }";
    }

}