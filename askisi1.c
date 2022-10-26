#include "askisi1.h"

// Function that creates the shared memory and the semaphores
void createResources(){

	// Creation of shared memory
	shm_id = shmget(SHMKEY, sizeof(SharedMem), 0600 | IPC_CREAT);
	if(shm_id < 0){
		perror("Shared Memory Failed!!\n");
		exit(2);
	}

	// Shared memory attachment
	ptrshmem = (SharedMem*) shmat(shm_id, (char *) 0, 0);
	if(ptrshmem == NULL){
		perror("Can't make shared mem attach\n");
		exit(2);
	}

	unsigned int value1 = 0, value2 = 1;

	// Creation of semaphores
	fullquery = sem_open("fullquery", O_CREAT | O_EXCL, 0644, value1);
	emptyquery = sem_open("emptyquery", O_CREAT | O_EXCL, 0644, value2);
	fullanswer = sem_open("fullanswer", O_CREAT | O_EXCL, 0644, value1);
	emptyanswer = sem_open("emptyanswer", O_CREAT | O_EXCL, 0644, value2);

	// Free semaphores
	sem_unlink("fullanswer");
	sem_unlink("emptyquery");
	sem_unlink("emptyanswer");
	sem_unlink("fullquery");

}

// Function that check if the arguments given from the command line are right
void check_arguments(char *filename,int procs,int N){
	// checks if the number is >0 
	if(procs<0 || N<0 ){
		printf("we need numbers >0 , programm is terminated \n");
		exit(3);
	}


	FILE *filep;
	filep = fopen(filename, "r");
	// checks if the file exists
	if (filep == NULL)
    {
        printf("Could not open file %s , programm is terminated", filename);
		exit(3);
    }

	// Close the file
	fclose(filep);
}

// Function that returns the number of rows contained within a file
int number_of_rows(char *filename) {
	char c;
	int count = 1;
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 0;
    }
  
    // Extract characters from file and store in character c
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;
  
    // Close the file
    fclose(fp);
    printf("The file %s has %d lines\n ", filename, count);
  
    return count;
}

// Function that returns a specific line from the file 
void specific_line(char *filename,int x,char * string){
	int lineNumber = x;

	
	FILE *file = fopen(filename, "r");
	int count = 0;
	if ( file != NULL )
	{
		char line[101]; 
		while (fgets(line, sizeof line, file) != NULL) // read a line until the file ends
		{
			if (count == lineNumber)
			{
				strcpy(string,line);
				// Close the file
				fclose(file);
				return;
			}
			else
			{
				count++;
			}
		}
		fclose(file);
	}
	else
	{
		//file doesn't exist
		return;
	}
	
	
}

// Function that contains the code that each child executes
void child_code(int filelinenumber,int N){
	int i, linerequest;
	char string[101];
	double avgtime;
	clock_t t1,t2;

	//printf("paididd %d \n",filelinenumber);
	for(i=0;i<N;i++){
		
		// random line of the file
		linerequest = rand() % filelinenumber ;

		// semaphore bind the memory
		sem_wait(emptyquery);
		printf("child %d to parent: %d \n",getpid(),linerequest+1);

		// begin count the execution time
		t1 = clock();
		usleep(10);

		// put the request in the shared memory
		ptrshmem->linenumber = linerequest;
		sem_post(fullquery);

		
		// Now waits for the parent's answer
		sem_wait(fullanswer);

		// take the parent's answer
		strcpy(string, ptrshmem->str); 

		// stop count the execution time
		t2 = clock();
		avgtime += ((double)(t2-t1)) * 1000.0 / CLOCKS_PER_SEC; 


		printf("parent to child: %d : %s \n",getpid(),string);

		// semaphores the shared memory is empty
		sem_post(emptyquery);
		sem_post(emptyanswer);


	}
	
	printf("Child: %d average time = %lf milli-seconds\n",getpid(),avgtime);

	shmdt(ptrshmem);

	exit(0);
}

// Function that frees the resources that was allocated
void free_resources(){
	// free the shared memory
	if(shmctl(shm_id,IPC_RMID,(struct shmid_ds *)NULL) < 0){
		perror("fail to free shared memory");
		exit(3);
	}

}


int main(int argc, char **argv){
	
	int procs, N, i, z, filelinenumber, linecount, num, status;
	
	char filename[100], line[101];
	
	pid_t pid;

	srand(time(NULL));
	
	// checks if the number of the arguments is right
	if (argc!=4){
		printf("Wrong arguments. Programm is terminated \n");
		exit(3);
	}else{
		procs = atoi(argv[2]);
		N = atoi(argv[3]);
		strcpy(filename, argv[1]);
		check_arguments(filename,procs,N); 	// checks the arguments input
	}
	

	// counts the number of the lines in the file
	linecount = number_of_rows(filename);
	
	// creates the shared memory and the semaphores
	createResources();
	

	// children creation
	for(i=0;i<procs;i++){
		pid = fork();
		if(pid == 0){
			
			child_code(linecount, N);
			break;
		}
		
	}	

	
	for(z=0;z<procs*N;z++){

		// take the childs question
		sem_wait(fullquery);
		filelinenumber = ptrshmem->linenumber;

		// find the line the child asks
		specific_line(filename,filelinenumber,line);

		sem_wait(emptyanswer);

		// give the line to the shared memory
		strcpy(ptrshmem->str,line);
		sem_post(fullanswer);

		
	}
		


	for(i=0;i<procs;i++){
		pid = wait(&status);
		printf("child with pid: %d is finished \n",pid);
	}

	free_resources();
}