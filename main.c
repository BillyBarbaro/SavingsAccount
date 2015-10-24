#include "main.h"

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