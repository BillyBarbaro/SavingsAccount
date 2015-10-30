#include "main.h"

void fork_failure() {
	perror("fork() failure");
	exit(EXIT_FAILURE);
}

void customer_enter(char* customer_type, char *amount) {
	execl(customer_type, customer_type, amount, NULL);
	exit(EXIT_SUCCESS);
}

void example1() {
	int thread_count, i;
	pid_t pid;

	thread_count = 0;

	if ((pid = fork()) == 0) {
		customer_enter("depositor", "200");
		exit(EXIT_SUCCESS);
	}
	else if (pid < 0) {
		fork_failure();
	}
	thread_count++;
	sleep(1);
	if ((pid = fork()) == 0) {
		customer_enter("withdrawer", "800");
		exit(EXIT_SUCCESS);
	}
	else if (pid < 0) {
		fork_failure();
	}
	thread_count++;
	sleep(1);
	if ((pid = fork()) == 0) {
		customer_enter("withdrawer", "200");
		exit(EXIT_SUCCESS);
	}
	else if (pid < 0) {
		fork_failure();
	}
	thread_count++;
	sleep(1);
	if ((pid = fork()) == 0) {
		customer_enter("depositor", "300");
		exit(EXIT_SUCCESS);
	}
	else if (pid < 0) {
		fork_failure();
	}
	thread_count++;
  sleep(1);
  if ((pid = fork()) == 0) {
          customer_enter("withdrawer", "1000");
          exit(EXIT_SUCCESS);
  }
  else if (pid < 0) {
          fork_failure();
  }
	thread_count++;
  sleep(1);
  if ((pid = fork()) == 0) {
          customer_enter("depositor", "100");
          exit(EXIT_SUCCESS);
  }
  else if (pid < 0) {
          fork_failure();
  }
	thread_count++;
  sleep(1);
  if ((pid = fork()) == 0) {
          customer_enter("depositor", "200");
          exit(EXIT_SUCCESS);
  }
  else if (pid < 0) {
          fork_failure();
  }
	thread_count++;
  sleep(1);
  if ((pid = fork()) == 0) {
          customer_enter("withdrawer", "500");
          exit(EXIT_SUCCESS);
  }
  else if (pid < 0) {
          fork_failure();
  }
	thread_count++;
  sleep(1);
  if ((pid = fork()) == 0) {
          customer_enter("depositor", "600");
          exit(EXIT_SUCCESS);
  }
  else if (pid < 0) {
          fork_failure();
  }
	thread_count++;
  sleep(1);
  if ((pid = fork()) == 0) {
          customer_enter("depositor", "300");
          exit(EXIT_SUCCESS);
  }
  else if (pid < 0) {
          fork_failure();
  }
	thread_count++;
  sleep(1);
  if ((pid = fork()) == 0) {
          customer_enter("depositor", "500");
          exit(EXIT_SUCCESS);
  }
  else if (pid < 0) {
          fork_failure();
  }
	thread_count++;

	for (i = 0; i < thread_count; i++) {
		wait(0);
	}
}

int main () {
	int semid, shmid;
	unsigned short seminit[NUM_SEMS];
	struct common *shared;
	union semun semctlarg;
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

	example1();

	semctl(semid, NUM_SEMS, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);

	return EXIT_SUCCESS;
}
