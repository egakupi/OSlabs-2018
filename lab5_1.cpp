#include <iostream>
#include <time.h>
#include <cstdio>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define NAME "/sem"
#define NAME1 "/sem1"
#define SHM_NAME "/shmn"
#define SHM_SIZE 50

using namespace std;

int flag = 1;

sem_t *semwrite;
sem_t *semread;

int fd;
int *addr;

void *func(void *vptr_args) {
	int a;
	while (flag){
		sem_wait(semwrite);
		a = *((int*)addr);
		cout << a << endl;
	    sem_post(semread);
	}		
}



int main() {
	pthread_t thread;
	fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0777);
	ftruncate(fd, 8);
	addr = (int*)mmap(0, 8, PROT_WRITE, MAP_SHARED, fd, 0);
	semread = sem_open(NAME, O_CREAT, 0644, 0);
	semwrite = sem_open(NAME1, O_CREAT, 0644, 0);
	pthread_create(&thread,NULL,func, &flag);
	getchar();
	flag = 0;
	void *res;
	pthread_join(thread, NULL);

	sem_close(semread);
	sem_unlink(NAME);
	sem_close(semwrite);
	sem_unlink(NAME1);
	munmap(&addr,8);
	close(fd);
		
	return 0;
}