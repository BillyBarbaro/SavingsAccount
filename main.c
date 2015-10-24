#include "main.h"

customer* create_customer(int withdrawl_amount) {
	customer *new_customer = malloc(sizeof(customer));
	new_customer->withdrawl_amount = withdrawl_amount;

	return new_customer;
}

node* create_node(customer *value) {
	node *new_node = malloc(sizeof(node));
	new_node->value = value;

	return new_node;
}

void add_customer_to_queue(linked_list *queue, int withdrawl_amount) {
	customer *new_customer = create_customer(withdrawl_amount);
	node *new_node = create_node(new_customer);
	new_node->next = queue->tail;
	queue->tail = new_node;
}

void serve_first_in_queue(linked_list *queue) {
	node *current_head = queue->head;
	node *new_head = current_head->next;
	queue->head = new_head;
}

int first_customer_amount(linked_list *queue) {
	customer *first_customer = queue->head->value;
	return first_customer->withdrawl_amount;
}

int main () {

	int semid, shmid;
	unsigned short seminit[NUM_SEMS];
	common *shared;
	union semun semctlarg;

	semid = semget(SEMKEY, NUM_SEMS, 0777 | IPC_CREAT);
	seminit[SEM_MUTEX]=1;
	seminit[SEM_WAITLIST]=0;
	semctlarg.array = seminit;
	semctl(semid, NUM_SEMS, SETALL, semctlarg);

	shmid = shmget(SHMKEY, sizeof(struct common), 0777 | IPC_CREAT);
	shared=(struct common *)shmat(shmid, 0, 0);
	shared->wait_count = 0;
	shared->balance = 500;
	shared->queue = malloc(sizeof(linked_list));

	semctl(semid, NUM_SEMS, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);

	return EXIT_SUCCESS;
}