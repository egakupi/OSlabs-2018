#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>

using namespace std;

struct argument {
    pthread_mutex_t mutex;
    bool flag;
};

void* funcThread1(void* args) {
    cout << "\nThe First thread started. . .\n";

    argument *arg = (argument *) args;

    while (arg->flag) {
        if (!pthread_mutex_trylock(&arg->mutex)) {
            for(int i = 0; i < 10; i++) {
                cout << '1' << flush;
                usleep(1);
            }
            pthread_mutex_unlock(&arg->mutex);
        }
        usleep(1);
    }

    pthread_exit((void*)150);
}

void* funcThread2(void* args) {
    cout << "The Second thread started. . .\n";

    argument *arg = (argument *) args;

    while (arg->flag) {
        if (!pthread_mutex_trylock(&arg->mutex)) {
            for(int i = 0; i < 10; i++) {
                cout << '2' << flush;
                usleep(1);
            }
            pthread_mutex_unlock(&arg->mutex);
        }
        usleep(1);
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
