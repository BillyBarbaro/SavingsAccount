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
#define LLKEY 100

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

typedef struct common {
	int wait_count;
	int balance;
	int linked_list_offset;
	int head_offset;
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

customer* create_customer(int withdrawl_amount, int offset) {

	int shmid;

	shmid = shmget(offset, sizeof(struct customer), 0777 | IPC_CREAT);
	customer *new_customer=(struct customer *)shmat(shmid, 0, 0);

	new_customer->withdrawl_amount = withdrawl_amount;

	return new_customer;
}

node* create_node(customer *value, int offset) {

	int shmid;

	shmid = shmget(offset, sizeof(struct node), 0777 | IPC_CREAT);
	node *new_node=(struct node *)shmat(shmid, 0, 0);

	new_node->value = value;

	return new_node;
}

void add_customer_to_queue(node *head, int withdrawl_amount, int offset) {
	offset = offset + 1;
	customer *new_customer = create_customer(withdrawl_amount, offset);
	offset = offset + 1;
	node *new_node = create_node(new_customer, offset);
	if (head == NULL) {
		printf("New head.\n");
		head = new_node;
	}
	else {
		printf("Adding node.\n");
		node *ptr = head;
		while (ptr->next != NULL) {
			ptr = ptr->next;
		}
		ptr->next = new_node;
	}
	printf("Addition finished.\n");
}

void serve_first_in_queue(node *current_head) {
	node *new_head = current_head->next;
	current_head = new_head;
}

int first_customer_amount(int offset) {
	offset = offset - 1;
	int shmid = shmget(offset, 0, 0);
	customer *first_customer = (customer *)shmat(shmid, 0, 0);
	//customer *first_customer = node->value;
	return first_customer->withdrawl_amount;
}