#include "stdio.h"
#include "sys/types.h"
#include "stdlib.h"
#include "unistd.h"

void main(int argc, char* const argv[]) {
    int i = 0;
    pid_t x;

    for (i=0;i<3;i++)        
        x = fork();
    printf("\tHello PID: [%d], PPID: [%d]\n", getpid(), getppid());
    // while(1);
}