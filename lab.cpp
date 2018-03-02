#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdio>

using namespace std;

void* funcThread1(void* args) {
    cout << "\nThe First thread started. . .\n";
    int arg = *((int*) args);

    while (*(bool*)args) {
        cout << '1' << flush;
        sleep(1);
    }
    pthread_exit((void*)150);
}

void* funcThread2(void* args) {
    cout << "The Second thread started. . .\n";
    int arg = *((int*) args);
    
    while (*(bool*) args) {
        cout << '2' << flush;
        sleep(1);
    }
    pthread_exit((void*)10);
}

int main() {
    pthread_t thr1;
    int statusThr1 = 1;
    bool flag1 = true;
    
    pthread_t thr2;
    int statusThr2 = 1;
    bool flag2 = true;
    
    
    pthread_create(&thr1, NULL, funcThread1, &flag1);
    
    pthread_create(&thr2, NULL, funcThread2, &flag2);

    cout << "Press any key (ENTERR) to stop it\n\n";
    
    getchar();
    flag1 = 0;
    flag2 = 0;
    pthread_join(thr1, (void**)&statusThr1);
    pthread_join(thr2, (void**)&statusThr2);
    cout << "\nThe First thread ended with " << statusThr1 << " code";
    cout << "\nThe Second thread ended with " << statusThr2 << " code"; 
    cout << "\nEnd of this shit\n";
    return 0;
}