/*
 -part b-

This part supports the parallel programming by using processes and shared memory.
As a difference to the first, in this part the coefficients array, the results array are saves on the shared memory to let deferent processes access to the same values. (the value defined locally because the variable with its constant duplicates in the process creation).
The reason to this change is the fact that each process runs independently and the results that each process calculate kept only in its memory environment. (the coefficients array also saved there to avoid memory waist and duplicating it each time)
The program creates a key to the memory and create a stock in the size of the maximum sells for two 4-degree polynomials.   Then defines two pointers for each array.
The program creates processes as the number of sells in the coefficients array that aren’t empty, (the key to the defined shared memory duplicates to every process and it also connects to the memory automatically). The program let each process to calculate its result when the index represents the degree.


*/

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <unistd.h>

#include <sys/wait.h>

#include <math.h>

/*runs through the given string, finds the indexes of the polynomial and the value(after , )
 saves the coefficients on the shared memory on the index of its degree.
counts how many term are in the polynomial to calculate=num of processes needed */
void getP(char * , int * , int * , int * );

#define MAX_DEG 4
#define MAX_SIZE 512

int main() {
  char stringInput[MAX_SIZE];
  int val;
  int * ptrP; //pointers for the shared memory p-polynom(saves the coefficients), R-results
  int * ptrR;
  key_t key;
  int shm_id; //for the creation of the sheared memory
  int pNum = 0, sum, i;
  pid_t child;

  do {
    puts("Please enter a polynomial and a value at this format: POLYNOM, VAL");
    fgets(stringInput, MAX_SIZE, stdin);
    stringInput[strlen(stringInput) - 1] = '\0'; //removing \n

    if ((strcmp(stringInput, "done") == 0))
      break;

    //creates the sheared memory where the polynomial and the results will be saved:
    key = ftok("/tmp", 'a');
    //allocating memory for the coefficients and the results arrays
    // in the max size of the polynomial for each of them
    if ((shm_id = shmget(key, 2 * MAX_DEG * sizeof(int), IPC_CREAT | 0600)) == -1) {
      perror("couldnt create memory");
      exit(1);
    }
    //creating the pointer for the memory where the polynomial will be stored
    if ((ptrP = (int * ) shmat(shm_id, NULL, 0)) < 0) {
      perror("failet to attach memory to pointer");
      exit(1);
    }
    ptrR = ptrP + MAX_DEG; //the place where the results will be stored

    getP(stringInput, ptrP, & val, & pNum);

    //cresting the processes(only if theres something to calculate)
    sum = 0;
    for (i = 1; i < MAX_DEG; ++i) {
      if (ptrP[i] != 0) {
        child = fork();
        if (child == -1) {
          perror("process creation failed\n");
          exit(1);
        }
        if (child == 0) {
          //each process calculates a term and saves the outcomes on the shered memort
          ptrR[i] = ptrP[i] * (int) pow(val, i);
          //freeing the processes' pointers and its resources
          shmdt(ptrP);
          shmdt(ptrR);
          exit(0);
        }
      }
    }

    //parent:
    //waiting for all processes
    for (int j = 0; j < pNum; j++)
      wait(NULL);

    for (int j = 1; j < MAX_DEG; ++j)
      sum += ptrR[j];

    sum += ptrP[0]; //free term

    printf("The output of the polynomial and the value- %s is: %d\n\n", stringInput, sum);

    //parent frees the pointers and deletes the shared memory for this polynomial
    shmdt(ptrP);
    shmdt(ptrR);
    shmctl(shm_id, IPC_RMID, NULL);

  } while (1);

  return 0;
}

void getP(char * stringInput, int * ptrP, int * val, int * pNum) {
  char * temp;
  int coeff, deg, startInd = 0, endInd = 0, x;

  while (stringInput[endInd] != ',') //end of polynom
  {
    if (stringInput[endInd] == '+') {
      endInd++;
      x = endInd; //saves the last index to keep the start of the free term
      startInd = endInd;
      continue;
    }

    if (stringInput[endInd] == 'x') {
      * pNum = * pNum + 1;
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
        ptrP[deg] = coeff;
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
      ptrP[deg] = coeff;
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
      ptrP[0] = atoi(temp);
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
  * val = atoi(temp);
  free(temp);
}
