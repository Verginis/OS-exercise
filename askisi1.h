#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
// #include <sys/ipc.h>



#define SHMKEY (key_t)1987




typedef struct{
	int linenumber;
	char str[101];
}SharedMem;


SharedMem * ptrshmem;
int shm_id;

//semaphores

sem_t* fullquery;
sem_t* emptyquery;
sem_t* fullanswer;
sem_t* emptyanswer;





// functions

void createResources();

void check_arguments(char *filename,int procs,int N);

void childcode(int filelinenumber, int N);

int number_of_rows(char *filename);

void specific_line(char *filename,int x,char * string);

void child_code(int filelinenumber,int N);

void free_resources();
