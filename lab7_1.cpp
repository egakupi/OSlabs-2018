#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>
#include <unistd.h>
#include <fcntl.h>           // For O_* constants 
#include <sys/stat.h>        // For file mode constants
#include <string.h>          // For memset();

struct thread_arg{
    bool  thread_ended;
    mqd_t mq_desc;
};

void* func1_thread(void* vp_arg){
    thread_arg* arg = (thread_arg*) vp_arg;
    char buf;
    unsigned msg_prio = 0;
    while(!(arg->thread_ended)) {
        memset(&buf, '\0', sizeof(char));
        if (mq_receive(arg->mq_desc, &buf, sizeof(buf), &msg_prio) == -1) { 
            perror("mq_receive");
        }
        printf("Message with priority %u say: %c\n", msg_prio, buf);
        fflush(stdout);
    }
    pthread_exit((void*) "Read thread ended");
}

int main()
{
    printf("Reader is started!\n");
    pthread_t thread1; 
    thread_arg thread1_arg;
    thread1_arg.thread_ended = false;
    
    mq_attr mq_attributes;
    mq_attributes.mq_flags   = O_NONBLOCK;   // Flags: 0 or O_NONBLOCK 
    mq_attributes.mq_maxmsg  = 10;           // Max. # of messages on queue
    mq_attributes.mq_msgsize = sizeof(char); // Max. message size (bytes)
    mq_attributes.mq_curmsgs = 0;            // # of messages currently in queue
    mqd_t mq_desc = mq_open("/MessageQueue", O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &mq_attributes);
    if (mq_desc == -1) {
            perror("mq_open");
    }

    thread1_arg.mq_desc = mq_desc;
    if (pthread_create(&thread1, NULL, func1_thread, &thread1_arg)) {
        perror("pthread_create");
    }
        
    getchar();
    thread1_arg.thread_ended = true;

    char* exit_thread1_code;
    if (pthread_join(thread1, (void**) &exit_thread1_code)) {
        return 1;
    }
    printf("\n%s\n", exit_thread1_code);
    if (mq_close(mq_desc)) {
        perror("mq_close");
    }
    return 0;
}