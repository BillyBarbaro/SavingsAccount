#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	common *shared;
	int deposit_amount;

	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	deposit_amount = atoi(argc[1]);

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = common *shmat(shmid, 0, 0);

	//Deposit code here
	P(semid, SEM_MUTEX);
	shared->balance = shared->balance + deposit;
	if (shared->wait_count == 0) {
		V(semid, SEM_MUTEX);
	}
	else if (first_customer_amount(shared->queue) > shared->balance) {
		V(semid, SEM_MUTEX);
	}
	else {
		V(semid, SEM_WAITLIST);
	}

	return EXIT_SUCCESS;
}