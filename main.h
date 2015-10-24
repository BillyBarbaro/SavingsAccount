#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>

#define SEMKEY 77
#define SHMKEY 77

#define NUM_SEMS   2
#define SEM_MUTEX  0
#define SEM_WAITLIST 1

typedef struct customer {
	int withdrawl_amount;
} customer;

typedef struct node {
	customer *value;
	struct node *next;
} node;

typedef struct linked_list
{
	node *head;
	node *tail;
} linked_list;

typedef struct common {
	int wait_count;
	int balance;
	linked_list *queue;
} common;

void P(int semid, int semaphore) {
	struct sembuf psembuf;

	psembuf.sem_op = -1;
	psembuf.sem_flg = 0;
	psembuf.sem_num = semaphore;
	semop(semid, &psembuf, 1);
	return;
}

void V(int semid, int semaphore) {
	struct sembuf vsembuf;

	vsembuf.sem_op = 1;
	vsembuf.sem_flg = 0;
	vsembuf.sem_num = semaphore;
	semop(semid, &vsembuf, 1);
	return;
}

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
	printf("Adding customer.\n");
	customer *new_customer = create_customer(withdrawl_amount);
	printf("Customer created.\n");
	node *new_node = create_node(new_customer);
	printf("Node created.\n");
	if (queue->tail == NULL) {
		printf("Null\n");
	}
	else {
		printf("Fine\n");
	}
	new_node->next = queue->tail;
	printf("Next set.\n");
	queue->tail = new_node;
	printf("Addition finished.\n");
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