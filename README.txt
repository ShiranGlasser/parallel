-PARALLEL- 
Authored by Shiran Glasser

==Description==
The program receives polynomials from the user and a value, then calculates the result of each value on each polynomial.
The program first runs through the received polynom and analyzes it. It saves all the coefficients as numbers in a designated array and the given value as an int in a  designated variable.
The program implements the parallel programming principle- first by using threads and global variables, and next- by using processes and shared memory. 
we will revie each approach:    
-part a-
This part supports the parallel programming by using threads.
The coefficients array, the results array and the value are defining globally to let deferent threads access to the same values.
(in the threads creation, the local variables aren’t duplicating).
The program creates threads as the number of sells in the coefficients array that aren’t empty- meaning: the number of terms to calculate in the current polynomial, the degrees that exists. 
At next- it sends each thread to a calculating method, where the thread approaches the global array in the given degree and calculate this coefficient with this degree for the value and saves the outcome on the global array.  
-part b-
This part supports the parallel programming by using processes and shared memory.
As a difference to the first, in this part the coefficients array, the results array are saves on the shared memory to let deferent processes access to the same values. (the value defined locally because the variable with its constant duplicates in the process creation).
The reason to this change is the fact that each process runs independently and the results that each process calculate kept only in its memory environment. (the coefficients array also saved there to avoid memory waist and duplicating it each time)
The program creates a key to the memory and create a stock in the size of the maximum sells for two 4-degree polynomials.   Then defines two pointers for each array.
The program creates processes as the number of sells in the coefficients array that aren’t empty, (the key to the defined shared memory duplicates to every process and it also connects to the memory automatically). The program let each process to calculate its result when the index represents the degree.
==functions: ==
getP-  runs through the given string, finds the indexes of the polynomial and the value(after , ) saves the coefficients on the array on the index of its degree.
in part b- also counts how many term are in the polynomial to calculate=num of processes needed 
calculate- only in part a- the function each thread will call. gets the index current term's degree
calculates the coefficient in that index location on the global array, with the glabal value in the power of the index.
==How to compile?==
Part a:
gcc polynomial_a.c -lm -o pA-lpthread
run: ./pA
Part b:
gcc polynomial._b.c -lm -o pB
run: ./pB
==Input:==
Polynomials and values until the word "done". In this pattern: "POLYNOM, VAL".
Assume that the input is only by this pattern and the polynomial is only until 4 degree.
The program supports only + operator.
==Output:==
"The output of the polynomial and the value- < > is: < > ".