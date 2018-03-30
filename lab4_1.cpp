#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <cstdio>
#include <fcntl.h>

using namespace std;

sem_t *sem;

struct argument {
    FILE *file;
    bool flag;
};


void* funcThread(void* args) {
    argument *arg = (argument *) args;

    cout << "\nThe first thread started. . .\n";
    char ch = '1';

    while (arg->flag) {
        sem_wait(sem);
        for(int i = 0; i < 10; i++) {
            fwrite((void*) &ch, sizeof(char), 1, arg->file);
            fflush(arg->file);
            cout << "1, " << flush;
            usleep(1000000);
        }
        sem_post(sem);
        usleep(2000000);
    }

    pthread_exit((void*) 777);
}

int main() {
    sem = sem_open("/name", O_CREAT, 0644, 1); // WHAT IS 0644?/???
    
    argument arg;
    arg.flag = true;
    arg.file = fopen("myFile.txt", "a");
    
    pthread_t thr;
    int statusThr = 1;
    pthread_create(&thr, NULL, funcThread, (void *) &arg);

    cout << "Press ENTERR to stop it\n";
    getchar();

    arg.flag = false;
    pthread_join(thr, (void **) &statusThr);
    cout << "\nThe first thread ended with " << statusThr << " code\n";

    fclose(arg.file);
    sem_close(sem);
    sem_unlink("/name");
    return 0;
}