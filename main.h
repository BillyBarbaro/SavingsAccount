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
#define SHMKEY 255
#define LINESTART 256

#define NUM_SEMS   2
#define SEM_MUTEX  0
#define SEM_WAITLIST 1

struct customer {
	struct customer *next;
	int amount_requested;
};

struct common {
	int wait_count;
	int balance;
	int customer_offset;
	int front_of_line;
};

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

struct customer* create_customer(int withdrawl_amount, int offset) {

	int shmid;
	struct customer *new_customer;

	shmid = shmget(offset, sizeof(struct customer), 0777 | IPC_CREAT);
	if (shmid < 0) {
		perror("Could not get shared memory");
		exit(EXIT_FAILURE);
	}
	new_customer=(struct customer *)shmat(shmid, 0, 0);

	new_customer->amount_requested = withdrawl_amount;
	new_customer->next = NULL;

	return new_customer;
}

void add_customer_to_queue(struct customer *first_customer, int withdrawl_amount, int offset) {
	struct customer *new_customer = create_customer(withdrawl_amount, offset);
	if (first_customer == NULL) {
		first_customer = new_customer;
	}
	else {
		struct customer *ptr = first_customer;
		while (ptr->next != NULL) {
			ptr = ptr->next;
		}
		ptr->next = new_customer;
	}
}

void serve_first_in_queue(struct customer *first_customer, int offset) {
	struct customer *second_cutsomer = first_customer->next;
	first_customer = second_cutsomer;
	shmctl(offset, IPC_RMID, 0);
}

int first_customer_amount(struct customer *first_customer) {
	return first_customer->amount_requested;
}

int get_semid() {
	int semid;
	semid = semget(SEMKEY, NUM_SEMS, 0777);
	if (semid < 0) {
		perror("Could not get semaphores depositor");
		exit(EXIT_FAILURE);
  }
	return semid;
}

struct common * get_shared() {
	int shmid;

	shmid = shmget(SHMKEY, 0, 0);
	if (shmid < 0) {
		perror("Could not get shared memory");
		exit(EXIT_FAILURE);
  }
	return (struct common *)shmat(shmid, 0, 0);
}

struct customer* get_first_customer(int front_offset) {

	int shmid;

	shmid = shmget(front_offset, 0, 0);
	if (shmid < 0) {
		perror("Could not get shared memory");
		exit(EXIT_FAILURE);
  }
	return (struct customer *)shmat(shmid, 0, 0);
}

union semun {
	/* value for SETVAL */
	int val;
	/* buffer for IPC_STAT, IPC_SET */
	struct semid_ds *buf;
	/* array for GETALL, SETALL */
	unsigned short *array;
	/* Linux specific part: */
	/* buffer for IPC_INFO */
	struct seminfo *__buf;
};
