#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>
#include <unistd.h>
#include <fcntl.h>           // For O_* constants 
#include <sys/stat.h>        // For file mode constants

struct thread_arg {
    bool  thread_ended;
    mqd_t mq_desc;
};


void* func1_thread(void* vp_arg) {
    thread_arg* arg = (thread_arg*) vp_arg;
    char buf = 'A';
    while(!(arg->thread_ended)) {
        if (mq_send(arg->mq_desc, &buf, sizeof(buf), 1)) {
            perror("mq_send");
        }
        printf("%c was written\n", buf);
        if (buf == 'Z') {
            buf = 'A' - 1;
        }
        buf++;
        sleep(1);
    }
    pthread_exit((void*) "Writer thread ended");
}

int main()
{
    printf("Writer is started!\n");
    pthread_t thread1; 
    thread_arg thread1_arg;
    thread1_arg.thread_ended = false;

    mq_attr mq_attributes;
    mq_attributes.mq_flags   = O_NONBLOCK;   // Flags: 0 or O_NONBLOCK 
    mq_attributes.mq_maxmsg  = 10;           // Max. # of messages on queue 
    mq_attributes.mq_msgsize = sizeof(char); // Max. message size (bytes) 
    mq_attributes.mq_curmsgs = 0;            // # of messages currently in queue
    mqd_t mq_desc = mq_open("/MessageQueue", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &mq_attributes);
    if (mq_desc == -1) {
        perror("mq_desc");
    }

    thread1_arg.mq_desc = mq_desc;
    if (pthread_create( &thread1, NULL, func1_thread, &thread1_arg)) {
        perror("pthread_create");
    }
    getchar();
    thread1_arg.thread_ended = 1;

    //Exit section
    char* exit_thread1_code;
    if (pthread_join( thread1, (void**) &exit_thread1_code)) {
        return 1;
    }
    printf("\n%s\n", exit_thread1_code);
    if (mq_close(mq_desc)) { 
        perror("mq_close");
    }
    if (mq_unlink("/MessageQueue")) {
        perror("mq_unlink");
    }
    return 0;
}