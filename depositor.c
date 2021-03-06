#include "main.h"

void print_deposit_info(int amount, int balance) {
	printf("Depositing %d dollars.\n", amount);
	printf("Current balance %d dollars.\n", balance);
}

int main(int argc, char *argv[]) {

	int semid;
	struct common *shared;
	int deposit_amount;
	struct customer *first_customer;

	if (argc != 2) {
		perror("Please enter only one argument");
		exit(EXIT_FAILURE);
	}

	deposit_amount = atoi(argv[1]);

	semid = get_semid();
	shared = get_shared();

	// wait(mutex)
	P(semid, SEM_MUTEX);

	first_customer = get_first_customer(shared->front_of_line);
	shared->balance = shared->balance + deposit_amount;
	print_deposit_info(deposit_amount, shared->balance);
	if (shared->wait_count == 0 || first_customer_amount(first_customer) > shared->balance) {
		// signal(mutex)
		V(semid, SEM_MUTEX);
	}
	else {
		//signal(wait_list)
		V(semid, SEM_WAITLIST);
	}
	return EXIT_SUCCESS;
}
