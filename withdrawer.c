#include "main.h"

void increment_wait_count(struct common *shared) {
	shared->wait_count = shared->wait_count + 1;
	shared->customer_offset = shared->customer_offset + 1;
	if (shared->wait_count == 1) {
		shared->front_of_line = shared->front_of_line + 1;
	}
}

void print_info(int amount, int balance) {
	printf("Withdrawing\n");
	printf("Successfully withdrew %d dollars.\n", amount);
	printf("New balance %d dollars.\n", balance);
}

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
		increment_wait_count(shared);

		add_customer_to_queue(first_customer, withdraw_amount, shared->customer_offset);
		printf("Not enough money.  Withdrawer Waiting\n");

		//signal(mutex)
		V(semid, SEM_MUTEX);
		//wait(wait_list)
		P(semid, SEM_WAITLIST);

		first_customer = get_first_customer(shared->front_of_line);

		shared->balance = shared->balance - first_customer_amount(first_customer);
		serve_first_in_queue(first_customer, shared->front_of_line);
		shared->wait_count = shared->wait_count - 1;

		print_info(withdraw_amount, shared->balance);

		if (shared->wait_count > 0) {
			shared->front_of_line = shared->front_of_line + 1;
			first_customer = get_first_customer(shared->front_of_line);
		}

		if (shared->wait_count > 0 && first_customer_amount(first_customer) <= shared->balance) {
			//signal(wait_list)
			V(semid, SEM_WAITLIST);
		}
		else {
			//signal(mutex)
			V(semid, SEM_MUTEX);
		}
	}

	return EXIT_SUCCESS;
}
