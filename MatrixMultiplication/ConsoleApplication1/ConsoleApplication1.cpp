#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_ROWS_A 100 //rows of input [A]
#define NUM_COLUMNS_A 100 //columns of input [A]
#define NUM_ROWS_B 100 //rows of input [B]
#define NUM_COLUMNS_B 100 //columns of input [B]

void *print_message_function(void *ptr);

#define NUM_PARTITIONS 2
int partitionSize = (NUM_ROWS_A + 1) / NUM_PARTITIONS;

int A[NUM_ROWS_A][NUM_COLUMNS_A];
int B[NUM_ROWS_B][NUM_COLUMNS_B];
int C[NUM_ROWS_A][NUM_COLUMNS_B];
int BT[NUM_COLUMNS_B][NUM_ROWS_B];

void makeAB()
{
	for (int i = 0; i < NUM_ROWS_A; i++) {
		for (int j = 0; j < NUM_COLUMNS_A; j++) {
			A[i][j] = i + j;
		}
	}
	for (int i = 0; i < NUM_ROWS_B; i++) {
		for (int j = 0; j < NUM_COLUMNS_B; j++) {
			B[i][j] = i*j;
		}
	}
}

struct p {
	int partitionA; /* row */
	int partitionB; /* column */
};

void *runner(void *param); /* the thread */

int main(int argc, char *argv[]) {
	int i, j, count = 0;	

	makeAB();

	time_t  ctime,ltime;

	time(&ctime);
		//Code for calculating transpose of B
	for (int i = 0; i < NUM_COLUMNS_B; i++)
	{
		for (int j = 0; j < NUM_ROWS_B; j++)
			BT[i][j] = B[j][i];

	}

	for (i = 0; i < NUM_ROWS_A; i += partitionSize) {
		for (j = 0; j < NUM_COLUMNS_B; j += partitionSize) {
			struct p *data = (struct p *) malloc(sizeof(struct p));
			data->partitionA = i;
			data->partitionB = j;
			/* Now create the thread passing it data as a parameter */
			pthread_t tid;       //Thread ID
			pthread_attr_t attr; //Set of thread attributes
			//Get the default attributes
			pthread_attr_init(&attr);
			//Create the thread
			pthread_create(&tid, &attr, runner, data);
			//Make sure the parent waits for all thread to complete
			pthread_join(tid, NULL);
		}
	}

	time(&ltime);

	//Print out the resulting matrix
	for (i = 0; i < NUM_ROWS_A; i++) {
		for (j = 0; j < NUM_COLUMNS_B; j++) {
			printf("%d ", C[i][j]);
		}
		printf("\n");
	}
	printf("time diff: %d seconds", ltime - ctime);
	return 0;
}


//The thread will begin control in this function
void *runner(void *param) {
	struct p *data = (struct p *)param; // the structure that holds our data

	int partA = data->partitionA;
	int partB = data->partitionB;
	//Row multiplied by column
	int rowA = 0, rowB = 0;
	for (rowA = partA; rowA < partA + partitionSize && rowA < NUM_ROWS_A; rowA++)
	{
		for (rowB = partB; rowB < partB + partitionSize && rowB < NUM_ROWS_B; rowB++)
		{
			for (int k = 0; k < NUM_COLUMNS_B; k++)
			{
				C[rowA][rowB] += A[rowA][k] * BT[rowB][k];
			}
		}
	}

	//Exit the thread
	pthread_exit(0);
	return NULL;
}
