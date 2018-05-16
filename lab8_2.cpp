#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <string>
#include <string.h>
#include <vector>
#include <arpa/inet.h>

using namespace std;

pthread_t inThread, handleThread, waitThread;
int sock, conn;
bool inFlag = 1, handleFlag = 1, waitFlag = 1;
struct sockaddr_in addr;

void* inThreadFunc(void* args) {
    int randomNum;
    char buf[256];
    while(inFlag) {
        memset(buf, '\0', sizeof(buf));
        //randomNum = rand();
        //string str = to_string(randomNum);
        //strcpy(buf, str.c_str());
        bzero(buf, 256);
        sprintf(buf, "%d", rand());
        printf("You:\n\"%s\"\n", buf);
        send(sock, buf, strlen(buf), 0);
        sleep(1);
    }
}

void* handleThreadFunc(void* args) {
    char buf[256];
    while(handleFlag) {
        memset(buf, '\0', sizeof(buf));
        recv(sock, buf, sizeof(buf), 0);
        if(buf != '\0') {
            printf("Server msg:\n\"%s\"\n", buf);
            //cout << "Buf in handleThread " << buf << endl << flush;
        }
        sleep(1);
    }
}

void* waitThreadFunc(void* args) {
    int conn;
    while(waitFlag) {
        conn = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        if (conn >= 0) {
            cout << "Connect in waitThread OK" << endl << flush;
            pthread_create(&inThread, NULL, inThreadFunc, NULL);
            pthread_create(&handleThread, NULL, handleThreadFunc, NULL);
            pthread_exit((void*) 0);
        } else {
            cout << "Connect in waitThread doesn't exist" << endl << flush;
            sleep(1);
        }
    }
}

int main() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    pthread_create(&waitThread, NULL, waitThreadFunc, NULL);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << "Press ENTER to stop it" << endl;
    getchar();
    inFlag = 0; handleFlag = 0; waitFlag = 0;
    pthread_join(inThread, NULL);
    pthread_join(handleThread, NULL);
    pthread_join(waitThread, NULL);
    shutdown(conn, 2);
    close(conn);
    close(sock);
    cout << "Completed correctly!\n";
    return 0;
}