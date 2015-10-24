#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	common *shared;
	int withdraw_amount;

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = (common *)shmat(shmid, 0, 0);

	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	withdraw_amount = atoi(argv[1]);

	P(semid, SEM_MUTEX);
	printf("Trying to withdraw %d dollars", withdraw_amount);
	if (shared->wait_count == 0 && shared->balance > withdraw_amount) {
		printf("Withdrawing");
		shared->balance = shared->balance - withdraw_amount;
		printf("Successfully withdrew %d dollars.", withdraw_amount);
		printf("New balance %d dollars.", shared->balance);
		V(semid, SEM_MUTEX);
	}
	else {
		shared->wait_count = shared->wait_count + 1;
		add_customer_to_queue(shared->queue, withdraw_amount);
		printf("Current balance %d dollars.", shared->balance);
		printf("Withdrawer Waiting");
		V(semid, SEM_MUTEX);
		P(semid, SEM_WAITLIST);
		printf("Withdrawing");
		shared->balance = shared->balance - first_customer_amount(shared->queue);
		serve_first_in_queue(shared->queue);
		shared->wait_count = shared->wait_count - 1;
		printf("Successfully withdrew %d dollars.", withdraw_amount);
		printf("New balance %d dollars.", shared->balance);
		// Ask if this comparison is correct
		if (shared->wait_count > 0 && first_customer_amount(shared->queue) < shared->balance) {
			printf("Signlaing next withdrawer");
			V(semid, SEM_WAITLIST);
		}
		else {
			V(semid, SEM_MUTEX);
		}
	}

	return EXIT_SUCCESS;
}