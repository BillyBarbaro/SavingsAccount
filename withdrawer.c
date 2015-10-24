#include "main.h"

int main(int argc, char *argv[]) {

	int semid, shmid;
	common *shared;
	int withdraw_amount;
	node *head;

	printf("Withdraw called.\n");
	if (argc != 2) {
		// Raise error and exit
	}

	// error check here
	withdraw_amount = atoi(argv[1]);

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = (common *)shmat(shmid, 0, 0);

	shmid = shmget(shared->head_offset, 0, 0);
	head = (node *)shmat(shmid, 0, 0);

	P(semid, SEM_MUTEX);
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
		add_customer_to_queue(head, withdraw_amount, shared->linked_list_offset);
		shared->linked_list_offset = shared->linked_list_offset + 2;
		if (shared->wait_count == 1) {
			shared->head_offset = shared->head_offset + 2;
		}
		printf("Head offset to %d\n", shared->head_offset);
		printf("Current balance %d dollars.\n", shared->balance);
		printf("Withdrawer Waiting\n");
		V(semid, SEM_MUTEX);
		P(semid, SEM_WAITLIST);
		printf("Withdrawing");
		shared->balance = shared->balance - first_customer_amount(shared->head_offset);
		serve_first_in_queue(head);
		shared->wait_count = shared->wait_count - 1;
		printf("Successfully withdrew %d dollars.\n", withdraw_amount);
		printf("New balance %d dollars.\n", shared->balance);
		// Ask if this comparison is correct
		if (shared->wait_count > 0 && first_customer_amount(shared->head_offset) < shared->balance) {
			shared->head_offset = shared->head_offset + 2;
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