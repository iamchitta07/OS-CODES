#include "stdio.h"
#include "sys/types.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"

pid_t x[2] = {0, 0};

void fn(int sig) {
    printf("\tRecieved Signal: [%d], PPID: [%d]\n", sig, getpid());

    // signal(SIGQUIT,SIG_IGN);
    kill(x[0], SIGQUIT);
    kill(x[1], SIGQUIT);
}

int main(int argc, char* const argv[]) {

    int i = 0;

    signal(SIGINT, fn);

    do {
        x[i] = fork();
        i++;
    } while ((x[i] > 0) && (i < 2));

    if (x[1] > 0) {
        printf("\n\t within parent child %d ppid %d",getpid(),x[0]);
        printf("\n\t within parent child %d ppid %d",getpid(),x[1]);
        sleep(2);
        kill(x[0], SIGKILL);
        kill(x[1], SIGKILL);
        while(1);
    } else {
        printf("x[0]: [%d], x[1]: [%d]\n", x[0], x[1]);
        printf("\tWithin Child: [%d], PPID: [%d]\n", getpid(), getppid());
        while(1);
    }

    return EXIT_SUCCESS;
}