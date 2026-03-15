#include "stdio.h"      // printf
#include "stdlib.h"     // EXIT STATUS
#include "unistd.h"     // fork
#include "signal.h"     // signal
#include "sys/wait.h"   // wait
#include "sys/types.h"  // pid_t

void fn(int sig) {
    printf("\tRecieved Signal [%d], Death of Child\n", sig);
}

int main(int argc, char* const argv[]) {
    int exitStatus;
    signal(SIGCHLD, fn);

    pid_t pid = fork(), child;
    if(pid) {
        child = wait(&exitStatus);
        printf("Child Id: [%d] Exit Status: [%d]\n", child, exitStatus);
        while(1);
    } else {
        printf("Within Child PID: [%d]\n", getpid());
        kill(getpid(), SIGUSR1);
        exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
} 