#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>
#include <string.h>
#include <fcntl.h>

using namespace std;

struct argument {
    pthread_mutex_t mutex;
    bool flag;
};

int fd[2];

void* funcThread1(void* args) { //writer
    cout << "\nThe First thread started. . .\n";

    argument *arg = (argument *) args;
    char buf = 'A';

    while (arg->flag) {
        //write(fd[1], &buf, 1);
        //cout << "Writer = " << buf << endl;
        if (buf == 'Z') {
            buf = 'A' - 1;
        }
        buf++;
        usleep(100);
    }

    pthread_exit((void*)150);
}

void* funcThread2(void* args) { //reader
    cout << "The Second thread started. . .\n";

    argument *arg = (argument *) args;
    char buf;

    while (arg->flag) {
        memset(&buf, '\0', sizeof(char));
        int rc = read(fd[0], &buf, 1);
        if (rc == -1) {
            perror("read");
            usleep(1000000);
            continue;            
        }
        cout << "Reader = " << buf << endl;    
        usleep(1000000);
    }

    pthread_exit((void*)10);
}

int main() {
    pthread_t thr1;
    int statusThr1 = 1;

    pthread_t thr2;
    int statusThr2 = 1;

    argument arg;
    arg.flag = true;

    pipe(fd);

    fcntl(fd[0], F_SETFL, O_NONBLOCK);

    pthread_mutex_init(&arg.mutex, NULL);

    pthread_create(&thr1, NULL, funcThread1, (void *) &arg);
    pthread_create(&thr2, NULL, funcThread2, (void *) &arg);

    cout << "Press any key (ENTERR) to stop it\n\n";
    getchar();

    arg.flag = false;
    pthread_join(thr1, (void **) &statusThr1);
    pthread_join(thr2, (void **) &statusThr2);
    pthread_mutex_destroy(&arg.mutex);

    cout << "\nThe First thread ended with " << statusThr1 << " code";
    cout << "\nThe Second thread ended with " << statusThr2 << " code";
    cout << "\nEnd of this shit\n";
    return 0;
}
