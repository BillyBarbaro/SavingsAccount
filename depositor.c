#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	struct common *shared;
	int deposit_amount;
	struct customer *first_customer;

	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	deposit_amount = atoi(argv[1]);

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	if (shmid < 0) {
                perror("Could not get shared memory");
        }
	shared = (struct common *)shmat(shmid, 0, 0);

	P(semid, SEM_MUTEX);

	shmid = shmget(shared->front_of_line, 0, 0);
	first_customer = (struct customer *)shmat(shmid, 0, 0);

	shared->balance = shared->balance + deposit_amount;

	printf("Depositing %d dollars.\n", deposit_amount);
	printf("Current balance %d dollars.\n", shared->balance);
	if (shared->wait_count == 0) {
		V(semid, SEM_MUTEX);
	}
	else if (first_customer_amount(first_customer) > shared->balance) {
		V(semid, SEM_MUTEX);
	}
	else {
		V(semid, SEM_WAITLIST);
	}

	return EXIT_SUCCESS;
}
