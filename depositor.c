#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	common *shared;
	int deposit_amount;
	customer *first_customer;

	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	deposit_amount = atoi(argv[1]);

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = (common *)shmat(shmid, 0, 0);

	P(semid, SEM_MUTEX);

	shmid = shmget(shared->front_of_line, 0, 0);
	first_customer = (customer *)shmat(shmid, 0, 0);

	printf("Depositing %d dollars\n", deposit_amount);
	shared->balance = shared->balance + deposit_amount;

	printf("Head offset to %d\n", shared->customer_offset);
	if (shared->wait_count == 0) {
		printf("No one in line.\n");
		printf("Depositor exiting\n");
		V(semid, SEM_MUTEX);
	}
	else if (first_customer_amount(first_customer) > shared->balance) {
		printf("Current amount %d dollars.\n", shared->balance);
		printf("Not enough for waiting withdrawer\n");
		printf("Current customer needs %d dollars.\n", first_customer_amount(first_customer));
		printf("Depositor exiting\n");
		V(semid, SEM_MUTEX);
	}
	else {
		printf("Current amount %d dollars.\n", shared->balance);
		printf("Current customer needs %d dollars.\n", first_customer_amount(first_customer));
		printf("Depositor exiting\n");
		V(semid, SEM_WAITLIST);
	}

	return EXIT_SUCCESS;
}
