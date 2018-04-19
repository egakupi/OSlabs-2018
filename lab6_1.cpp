#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> // for usleep
 
#define NAMEDPIPE_NAME "/home/egakupi/Workspace/OSlabs/my_named_pipe" // need full path
#define BUFSIZE 1

int flag = 1;
int fd;

void* funcThread(void* args) { // writer
    printf("The reader's thread is started\n");
    char buf;
    while (flag) {
        memset(&buf, '\0', sizeof(char));
        if ((read(fd, &buf, BUFSIZE)) <= 0) {
            perror("read");
            return 0;
        }
        printf("%c was red\n", buf);
        usleep(1000);
    }
}

int main () {
    pthread_t thr;
    int statusThr = 1;

    // // mkfifo(NAMEDPIPE_NAME, O_RDONLY);

    if (mkfifo(NAMEDPIPE_NAME, 0666)) {
        perror("mkfifo");
    }
    
    if ((fd = open(NAMEDPIPE_NAME, O_RDONLY)) <= 0) {
        perror("open");
        return 1;
    }
    
    if (pthread_create(&thr, NULL, funcThread, NULL)) {
        perror("pthread_create");
        return 1;
    }

    printf("Press any key (ENTERRRR) to stop it\n");
    getchar();
    flag = 0;

    pthread_join(thr, (void **) &statusThr);
    printf("\nThe reader's thread is stopped with %d code", statusThr);
    close(fd);
    remove(NAMEDPIPE_NAME);
    return 0;
}
