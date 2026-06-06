# Game AI Project

Creating the best AI AI has ever seen



### Finite State Machine Rant
The FSM System ~~sucks~~ is bad.
The State Transitions Evaluations are std::functions that return, which would make sense if you wanted states with no 
data; but that's not the case. <br>
The best things to do now, is either pass a BlackboardComponent to the function, from FSM when calling the function. <br>
Which is an easy fix, but I am not sure if that fixes the mess of ugly code. <br>
Abstraction and encapsulation is the key to remove/hide ugly code and complexity.