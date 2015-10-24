#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	common *shared;
	int deposit_amount;

	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	deposit_amount = atoi(argv[1]);

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = (common *)shmat(shmid, 0, 0);

	P(semid, SEM_MUTEX);
	printf("Depositing %d dollars", deposit_amount);
	shared->balance = shared->balance + deposit_amount;
	if (shared->wait_count == 0) {
		printf("No one in line.");
		printf("Depositor exiting");
		V(semid, SEM_MUTEX);
	}
	else if (first_customer_amount(shared->queue) > shared->balance) {
		printf("Current amount %d dollars.", shared->balance);
		printf("Current customer needs %d dollars.", first_customer_amount(shared->queue));
		printf("Depositor exiting");
		V(semid, SEM_MUTEX);
	}
	else {
		printf("Current amount %d dollars.", shared->balance);
		printf("Current customer needs %d dollars.", first_customer_amount(shared->queue));
		printf("Depositor exiting");
		V(semid, SEM_WAITLIST);
	}

	return EXIT_SUCCESS;
}