package DFAs;

import java.util.*;

public class DFAB 
{

	// this one is Dfa for identifiers
    public static DFA createIDFA() 
    {
        DFA dfa = new DFA();
        dfa.setSS(0);

        for (char c = 'a'; c <= 'z'; c++) // check for characters(small case) 
        {
            dfa.addT(0, c, 1); // start to final if found 
           
            dfa.addT(1, c, 1); // loops over final 
        
     }

        dfa.setFS(1);
        return dfa;
    }

    
    //this one is DFA for Numbers 
    public static DFA createNDFA() 
    {
        DFA dfa = new DFA();
        
        dfa.setSS(0);

        for (char c = '0'; c <= '9'; c++) // iterators for numbers 
        {
            dfa.addT(0, c, 1); // Integer start
         dfa.addT(1, c, 1); // Continue for integer found 
            dfa.addT(2, c, 3); // First digit after decimal
            dfa.addT(3, c, 3); // Continue decimal part
        }
//For decimal
        
        dfa.addT(1, '.', 2);

        dfa.setFS(1); 
        dfa.setFS(3); 

        return dfa;
    }

    
    // This DFA is for Operators 
    public static DFA createODFA()
    {
        DFA dfa = new DFA();
        dfa.setSS(0);

        char[] op = {'=', '+', '-', '*', '/', '%', '^'};
        for (char o : op) 
        {
            dfa.addT(0, o, 1);
        }

        dfa.setFS(1);
        return dfa;
    }
}
