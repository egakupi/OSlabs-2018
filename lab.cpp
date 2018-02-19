#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>

using namespace std;

bool flag1 = true;
bool flag2 = true;

void* funcThread1(void* args) {
    while (flag1) {
        cout << '1';
        usleep(1000);
    }
}

void* funcThread2(void* args) {
    while (flag2) {
        cout << '2';
        usleep(1000);
    }
}

int main() {
    pthread_t thr1;
    pthread_t thr2;
    pthread_create(&thr1, NULL, funcThread1, NULL);
    pthread_create(&thr2, NULL, funcThread2, NULL);
    getchar();
    flag1 = false;
    flag2 = false;
    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);
    return 0;
}