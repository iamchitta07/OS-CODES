#include "stdio.h"
#include "sys/types.h"
#include "signal.h"
#include "unistd.h"
#include "stdlib.h"

// Handler for SIGINT
void fn1(int x) {
    printf("\tPID: [%d], Recieved Signal: [%d], PPID: [%d] and GID: [%d] \n", getpid(), x, getppid(), getpgid(0));
    signal(SIGINT, SIG_DFL);
    // kill(getpid(),SIGINT);
}

// Handler for SIGTSTP
void fn5(int x) {
    printf("\n\t Recieved Signal %d \n", x);
    printf("\n\t Hey!!! you are forcefully suspending me.....\n");
    signal(SIGTSTP, SIG_DFL);
}

int main(int argc, char *const argv[]) {
    signal(SIGINT, fn1);  // custom handler defined for SIGINT signal
    signal(SIGTSTP, fn5); // custom handler defined for SIGSTOP signal

    fork() && fork();
    printf("PID: [%d], PPID: [%d], GID: [%d]\n", getpid(), getppid(), getpgid(0));
    setpgid(0,0);
    printf("After setting GID, PID: [%d], PPID: [%d], GID: [%d]\n", getpid(), getppid(), getpgid(0));

    while(1);

    return EXIT_SUCCESS;
}