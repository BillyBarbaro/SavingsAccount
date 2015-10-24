#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	common *shared;
	int deposit_amount;
	node *head;

	printf("Deposit called.\n");
	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	deposit_amount = atoi(argv[1]);

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = (common *)shmat(shmid, 0, 0);

	shmid = shmget(shared->head_offset, 0, 0);
	head = (node *)shmat(shmid, 0, 0);

	P(semid, SEM_MUTEX);
	printf("Depositing %d dollars\n", deposit_amount);
	shared->balance = shared->balance + deposit_amount;

	printf("Head offset to %d\n", shared->head_offset);
	if (shared->wait_count == 0) {
		printf("No one in line.\n");
		printf("Depositor exiting\n");
		V(semid, SEM_MUTEX);
	}
	else if (first_customer_amount(shared->head_offset) > shared->balance) {
		printf("Current amount %d dollars.\n", shared->balance);
		printf("Current customer needs %d dollars.\n", first_customer_amount(shared->head_offset));
		printf("Depositor exiting");
		V(semid, SEM_MUTEX);
	}
	else {
		printf("Current amount %d dollars.\n", shared->balance);
		printf("Current customer needs %d dollars.\n", first_customer_amount(shared->head_offset));
		printf("Depositor exiting\n");
		V(semid, SEM_WAITLIST);
	}

	return EXIT_SUCCESS;
}