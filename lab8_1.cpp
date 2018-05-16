#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>

using namespace std;

pthread_t inThread, handleThread, outThread, waitThread;
bool inFlag = 1, handleFlag = 1, outFlag = 1, waitFlag = 1;
int listener, sock;
vector<string> inQueue, outQueue;
struct sockaddr_in addr;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER, mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* inThreadFunc(void *args) {
    char buf[256];
    while(inFlag) {
        memset(buf, '\0', sizeof(buf));
        int codeRecv = recv(sock, buf, sizeof(buf), 0);
        if (codeRecv > 0) {
            cout << "InThreadFunc recive " << buf << endl << flush;
            pthread_mutex_lock(&mutex1);
            inQueue.push_back(buf);
            pthread_mutex_unlock(&mutex1);
        } else if (codeRecv == 0) {
            cout << "Connection doesnt exist!\n";
            sleep(1);
        } else if (codeRecv < 0) {
            cout << "Connection error!\n";
            sleep(1);
        }
    }
}

void* handleThreadFunc(void *args) {
    char buf[256];
    while(handleFlag) {
        pthread_mutex_lock(&mutex1);
        if (inQueue.empty()){
            pthread_mutex_unlock(&mutex1);
            sleep(1);
            continue;
        } else {
            memset(buf, '\0', sizeof(buf));
            //strcpy(buf, inQueue[0].c_str()); // c_str returns with '\0' at the end
            inQueue.erase(inQueue.begin());
            pthread_mutex_unlock(&mutex1);
            //buf[0]++;
            sprintf(buf, "you say: %s", inQueue[0].c_str());
            cout << "Server handle: " << buf << endl << flush;            
            pthread_mutex_lock(&mutex2);
            outQueue.push_back(buf);
            pthread_mutex_unlock(&mutex2);
        }
    }
}

void* outThreadFunc(void *args) {
    char buf[256];
    while(outFlag) {
        pthread_mutex_lock(&mutex2);
        if (outQueue.empty()) {
            pthread_mutex_unlock(&mutex2);
            sleep(1);
            continue;
        } else {
            memset(buf, '\0', sizeof(buf));
            strcpy(buf, outQueue[0].c_str());
            outQueue.erase(outQueue.begin());
            pthread_mutex_unlock(&mutex2);
            int codeSend = send(sock, buf, sizeof(buf), 0);
            if (codeSend > 0) {
                cout << "Sended correctly" << endl << flush;
            } else {
                cout << "Send error" << endl << flush;
            }
        }
    }
}

void* waitThreadFunc(void *args) {
    while(waitFlag) {
        int sizeAddr = sizeof(addr);
        sock = accept(listener, (struct sockaddr*)&addr, (socklen_t*)&sizeAddr);
        if (sock > 0) {
            cout << "Accept in waitThread OOOOOKi" << endl << flush;
            pthread_create(&inThread, NULL, inThreadFunc, NULL);
            pthread_create(&handleThread, NULL, handleThreadFunc, NULL);
            pthread_create(&outThread, NULL, outThreadFunc, NULL);
            pthread_exit((void*) 0);
        }
        perror("Accept");
        sleep(1);
    }
}

int main() {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listener, F_SETFL, O_NONBLOCK);
    if (listener < 1) {
        cout << "ERROR opening socket listener in main" << endl << flush;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cout << "CHECK bind in main " << bind(listener, (struct sockaddr*)&addr, sizeof(addr)) << endl << flush;
    listen(listener, 1);
    pthread_create(&waitThread, NULL, waitThreadFunc, NULL);
    cout << "Press ENTER to stop it" << endl;
    getchar();
    inFlag = 0; handleFlag = 0; outFlag = 0; waitFlag = 0;
    pthread_join(inThread, NULL);
    pthread_join(handleThread, NULL);
    pthread_join(outThread, NULL);
    pthread_join(waitThread, NULL);
    shutdown(sock, 2); //
    close(sock);
    close(listener);
    cout << "Completed correctly!" << endl;
    return 0;
}