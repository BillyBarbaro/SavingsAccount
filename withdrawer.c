#include "main.h"

int main() {

	int semid, shmid;
	common *shared;

	semid = semget(SEMKEY, NUM_SEMS, 0777);
	shmid = shmget(SHMKEY, 0, 0);
	shared = common *shmat(shmid, 0, 0);

	//Withdrawl code here

	return EXIT_SUCCESS;
}