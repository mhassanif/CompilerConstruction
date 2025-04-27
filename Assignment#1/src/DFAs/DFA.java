package DFAs;

import java.util.*;

public class DFA 
{
    private Set<Integer> st;  // This is a Set to store all states 
    private int ss;           // This is for start state
    
    private Set<Integer> fs;  // This set is for final state 
   
    private Map<Integer, Map<Character, Integer>> tt; // This is the transition table 

    public DFA() 
    {
        this.st = new HashSet<>();
        this.fs = new HashSet<>();
        
        this.tt = new HashMap<>();
    }

    public void setSS(int start)
    {
        this.ss = start;
        this.st.add(start);
    }
    
//the method addT ensures that the state from has a transition map by initializing one if absent. Then, it adds a transition from from to to on the given input character and updates the state set st.
    public void addT(int from, char input, int to) {
        tt.putIfAbsent(from, new HashMap<>()); // ensures that the state from has a transition map; if absent, it initializes one.
        tt.get(from).put(input, to);
        st.add(from);
        st.add(to);
    }

    public void setFS(int state)
    {
        this.fs.add(state);
        this.st.add(state);
    }

    public boolean simulate(String input) {
        int cs = ss; 

        for (char symbol : input.toCharArray()) 
        {
            if (!tt.containsKey(cs) || !tt.get(cs).containsKey(symbol))
{
                return false; // IF DEAD STATE 
            }
            cs = tt.get(cs).get(symbol);
        }

        return fs.contains(cs);
    }

   /* public void displayDFA() 
    {
        System.out.println("DFA States:");
        for (Integer s : st)
        {
            System.out.println("State " + s + (fs.contains(s) ? " (Final)" : ""));
            if (tt.containsKey(s)) {
                for (Map.Entry<Character, Integer> entry : tt.get(s).entrySet()) {
                    System.out.println("  " + s + " --" + entry.getKey() + "--> " + entry.getValue());
                }
            }
        }
    }
*/
   
    public int getSS() 
    {
        return ss;
    }

    public Set<Integer> getFS() 
    {
        return fs;
    }

    public Map<Integer, Map<Character, Integer>> getTT() 
    {
        return tt;
    }
}
