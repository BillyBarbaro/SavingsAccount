#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	common *shared;
	int withdraw_amount;
	customer *first_customer;

	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	withdraw_amount = atoi(argv[1]);

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = (common *)shmat(shmid, 0, 0);

	P(semid, SEM_MUTEX);

	shmid = shmget(shared->front_of_line, 0, 0);
	first_customer = (customer *)shmat(shmid, 0, 0);

	printf("Trying to withdraw %d dollars\n", withdraw_amount);
	// Different from spec. > vs >=
	if (shared->wait_count == 0 && shared->balance >= withdraw_amount) {
		printf("Withdrawing\n");
		shared->balance = shared->balance - withdraw_amount;
		printf("Successfully withdrew %d dollars.\n", withdraw_amount);
		printf("New balance %d dollars.\n", shared->balance);
		V(semid, SEM_MUTEX);
	}
	else {
		shared->wait_count = shared->wait_count + 1;
		shared->customer_offset = shared->customer_offset + 1;
		if (shared->wait_count == 1) {
			shared->front_of_line = shared->front_of_line + 1;
		}
		add_customer_to_queue(first_customer, withdraw_amount, shared->customer_offset);
		printf("Current customer offset to %d\n", shared->customer_offset);
		printf("Current balance %d dollars.\n", shared->balance);
		printf("Withdrawer Waiting\n");
		V(semid, SEM_MUTEX);
		P(semid, SEM_WAITLIST);

		shmid = shmget(shared->front_of_line, 0, 0);
		first_customer = (customer *)shmat(shmid, 0, 0);

		printf("Withdrawing\n");
		shared->balance = shared->balance - first_customer_amount(first_customer);
		serve_first_in_queue(first_customer);
		shared->wait_count = shared->wait_count - 1;
		printf("Successfully withdrew %d dollars.\n", withdraw_amount);
		printf("New balance %d dollars.\n", shared->balance);
		if (shared->wait_count > 0) {
			shared->front_of_line = shared->front_of_line + 1;
			shmid = shmget(shared->front_of_line, 0, 0);
			first_customer = (customer *)shmat(shmid, 0, 0);
		}
		// Ask if this comparison is correct
		if (shared->wait_count > 0 && first_customer_amount(first_customer) <= shared->balance) {
			printf("Signlaing next withdrawer\n");
			V(semid, SEM_WAITLIST);
		}
		else {
			printf("No one in line or too much asked\n");
			V(semid, SEM_MUTEX);
		}
	}

	return EXIT_SUCCESS;
}
