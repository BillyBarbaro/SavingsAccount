#include "main.h"

void fork_failure() {
	perror("fork() failure");
	exit(EXIT_FAILURE);
}

void customer_enter(char* customer_type, char *amount) {
	execl(customer_type, customer_type, amount, NULL);
	exit(EXIT_SUCCESS);
}

int main () {
	int semid, shmid;
	unsigned short seminit[NUM_SEMS];
	struct common *shared;
	union semun semctlarg;
	pid_t pid1, pid2, pid3, pid4;
	struct customer *first_in_line;

	semid = semget(SEMKEY, NUM_SEMS, 0777 | IPC_CREAT);
	if (semid < 0) {
                perror("Could not get semaphores");
		exit(EXIT_FAILURE);
        }
	seminit[SEM_MUTEX]=1;
	seminit[SEM_WAITLIST]=0;
	semctlarg.array = seminit;
	semctl(semid, NUM_SEMS, SETALL, semctlarg);

	shmid = shmget(SHMKEY, sizeof(struct common), 0777 | IPC_CREAT);
	if (shmid < 0) {
                perror("Could not get shared memory");
       		exit(EXIT_FAILURE);
	 }
	shared=(struct common *)shmat(shmid, 0, 0);
	shared->wait_count = 0;
	shared->balance = 500;

	shmid = shmget(LINESTART, sizeof(struct customer), 0777 | IPC_CREAT);
	if (shmid < 0) {
                perror("Could not get shared memory");
		exit(EXIT_FAILURE);
        }
	first_in_line = (struct customer *)shmat(shmid, 0, 0);

	shared->customer_offset = LINESTART;
	shared->front_of_line = LINESTART;
	
	if ((pid1 = fork()) == 0) {
		customer_enter("depositor", "200");
	}
	else if (pid1 < 0) {
		fork_failure();
	}
	else if ((pid2 = fork()) == 0) {
		customer_enter("withdrawer", "800");
	}
	else if (pid2 < 0) {
		fork_failure();
	}
	else if ((pid3 = fork()) == 0) {
		customer_enter("withdrawer", "200");
	}
	else if (pid3 < 0) {
		fork_failure();
	}
	else if ((pid4 = fork()) == 0) {
		customer_enter("depositor", "300");
	}
	else if (pid4 < 0) {
		fork_failure();
	}

	wait(0);
	wait(0);
	wait(0);
	wait(0);

	semctl(semid, NUM_SEMS, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);

	return EXIT_SUCCESS;
}
