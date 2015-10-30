#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	struct common *shared;
	int withdraw_amount;
	struct customer *first_customer;

	if (argc != 2) {
		perror("Please enter only one argument");
		exit(EXIT_FAILURE);
	}

	withdraw_amount = atoi(argv[1]);

	semid = get_semid();
	shared = get_shared();

	P(semid, SEM_MUTEX);

	first_customer = get_first_customer(shared->front_of_line);

	printf("Withdrawing %d dollars.\n", withdraw_amount);
	if (shared->wait_count == 0 && shared->balance >= withdraw_amount) {
		shared->balance = shared->balance - withdraw_amount;
		printf("New balance %d dollars.\n", shared->balance);
		// signal(mutex)
		V(semid, SEM_MUTEX);
	}
	else {
		shared->wait_count = shared->wait_count + 1;
		shared->customer_offset = shared->customer_offset + 1;
		if (shared->wait_count == 1) {
			shared->front_of_line = shared->front_of_line + 1;
		}
		add_customer_to_queue(first_customer, withdraw_amount, shared->customer_offset);
		printf("Not enough money.  Withdrawer Waiting\n");
		V(semid, SEM_MUTEX);
		P(semid, SEM_WAITLIST);

		shmid = shmget(shared->front_of_line, 0, 0);
		first_customer = (struct customer *)shmat(shmid, 0, 0);

		printf("Withdrawing\n");
		shared->balance = shared->balance - first_customer_amount(first_customer);
		serve_first_in_queue(first_customer, shared->front_of_line);
		shared->wait_count = shared->wait_count - 1;
		printf("Successfully withdrew %d dollars.\n", withdraw_amount);
		printf("New balance %d dollars.\n", shared->balance);
		if (shared->wait_count > 0) {
			shared->front_of_line = shared->front_of_line + 1;
			shmid = shmget(shared->front_of_line, 0, 0);
			if (shmid < 0) {
				perror("Could not get shared memory");
				exit(EXIT_FAILURE);
      }
			first_customer = (struct customer *)shmat(shmid, 0, 0);
		}
		if (shared->wait_count > 0 && first_customer_amount(first_customer) <= shared->balance) {
			V(semid, SEM_WAITLIST);
		}
		else {
			V(semid, SEM_MUTEX);
		}
	}

	return EXIT_SUCCESS;
}
