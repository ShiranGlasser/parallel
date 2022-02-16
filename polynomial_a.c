/*
 -part a-

The program receives polynomials from the user and a value, then calculates the result of each value on each polynomial.
The program first runs through the received polynom and analyzes it. It saves all the coefficients as numbers in a designated array and the given value as an int in a  designated variable.

This part supports the parallel programming by using threads.
The coefficients array, the results array and the value are defining globally to let deferent threads access to the same values.
 The program creates threads as the number of sells in the coefficients array that aren’t empty- meaning: the number of terms to calculate in the current polynomial, the degrees that exists. 
At next- it sends each thread to a calculating method, where the thread approaches the global array in the given degree and calculate this coefficient with this degree for the value and saves the outcome on the global array.



*/

#include <stdio.h>

#include <string.h>

#include <pthread.h>

#include <stdlib.h>

#include <math.h>

/*runs through the given string, finds the indexes of the polynomial and the value(after , )
 saves the coefficients on the global array on the index of its degree.
*/
void getP(char *);

//the function each thread will call. gets the index current term's degree
//calculates the coefficient in that index location on the global array, with the glabal value in the power of the index
void * calculateP(void * );

#define MAX_DEG 4
#define MAX_SIZE 512
int global_results[MAX_DEG];
int globalVal;
int globalCoeffs[MAX_DEG];

int main() {
    char stringInput[MAX_SIZE];
    int sum, i, status;

    do {
        //clearing the global variables to the current polynomial
        for(int k=0; k<MAX_DEG; k++)
        {
         globalCoeffs[k]=0;
         global_results[k]=0;
        }

        puts("Please enter a polynomial and a value at this format: POLYNOM, VAL");
        fgets(stringInput, MAX_SIZE, stdin);
        stringInput[strlen(stringInput) - 1] = '\0'; //removing \n

        if ((strcmp(stringInput, "done") == 0))
            break;

        getP(stringInput);

        //cresting the threads and sending them to calculate the value of each term, and saves the results on the global array
        sum = 0;
        pthread_t threads[MAX_DEG];
        int inds[MAX_DEG];

        for (i = 1; i < MAX_DEG; ++i) {
            inds[i]=i;
            if (globalCoeffs[inds[i]] != 0) {
                status = pthread_create( &threads[inds[i]], NULL, calculateP, (void * ) & inds[i]);
                if (status != 0) {
                    perror("pthread create failed\n");
                    exit(1);
                }
            }
        }

        //waiting for all threads
        for (int j = 1; j < MAX_DEG; j++) {
            if (globalCoeffs[inds[j]] != 0) {
                pthread_join(threads[inds[j]], NULL);
            }
        }

        for (int j = 1; j < MAX_DEG; ++j)
            sum += global_results[j];

        sum += globalCoeffs[0];

        printf("The output of the polynomial and the value- %s is: %d\n\n", stringInput, sum);

    }
    while (1);

    return 0;
}

void getP(char * stringInput) {
    char * temp;
    int coeff, deg, startInd = 0, endInd = 0, x=0;

    while (stringInput[endInd] != ',') //end of polynom
    {
        if (stringInput[endInd] == '+') {
            endInd++;
            x = endInd; //saves the last index to keep the start of the free term
            startInd = endInd;
            continue;
        }

        if (stringInput[endInd] == 'x') {
            if (stringInput[endInd - 1] != '*')
                coeff = 1;

            else {
                endInd--;
                //find the coefficient of the current term:
                temp = (char * ) malloc((endInd - startInd + 1) * sizeof(char));
                if (temp == NULL) {
                    perror("There was a problem with the dynamic assigning\n");
                    exit(1);
                }
                memset(temp, '\0', sizeof(temp));
                strncpy(temp, stringInput + startInd, endInd - startInd);
                coeff = atoi(temp);
                free(temp);
            }

            //finds the degree:
            if (stringInput[endInd] == '*')
                endInd++;
            //endInd on x
            if (stringInput[endInd + 1] == ',' || stringInput[endInd + 1] == '+') //theres no degree to x
            {
                deg = 1;
                globalCoeffs[deg] = coeff;
                endInd++;
                startInd = endInd;
                continue;
            } else {
                endInd += 2; //on the degree(after ^)
                startInd = endInd;
                while (stringInput[endInd] != '+' && stringInput[endInd] != ',')
                    endInd++;
                temp = (char * ) malloc((endInd - startInd + 1) * sizeof(char));
                if (temp == NULL) {
                    perror("There was a problem with the dynamic assigning\n");
                    exit(1);
                }
                memset(temp, '\0', sizeof(temp));
                strncpy(temp, stringInput + startInd, endInd - startInd);
                //saving the value as an int of the current degree
                deg = atoi(temp);
                free(temp);
            }
            globalCoeffs[deg] = coeff;
            startInd = endInd;
            continue;
        }

        if (stringInput[endInd + 1] == ',') {
            //reached the end of the free term
            startInd = x;
            endInd++;
            //find the free term:
            temp = (char * ) malloc((endInd - startInd + 1) * sizeof(char));
            if (temp == NULL) {
                perror("There was a problem with the dynamic assigning\n");
                exit(1);
            }
            memset(temp, '\0', sizeof(temp));
            strncpy(temp, stringInput + startInd, endInd - startInd);
            globalCoeffs[0] = atoi(temp);
            free(temp);
            continue;
        }
        endInd++;
    }

    endInd += 2;
    startInd = endInd; //on the start of the value, after (, )
    while (endInd < strlen(stringInput))
        endInd++;

    //creating a substring the contains the chars of the value input
    temp = (char * ) malloc((endInd - startInd + 1) * sizeof(char));
    if (temp == NULL) {
        perror("There was a problem with the dynamic assigning\n");
        exit(1);
    }
    memset(temp, '\0', sizeof(temp));
    strncpy(temp, stringInput + startInd, endInd - startInd);
    //saving the value as an int on the given argument
    globalVal = atoi(temp);
    free(temp);
}

void * calculateP(void * argument) {
    int ind = * (int *) argument;
    double d = pow((double) globalVal, (double) ind);
    global_results[ind] = globalCoeffs[ind] * (int) d;
    return NULL;
}
