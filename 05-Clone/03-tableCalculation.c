#define _GNU_SOURCE
#include "sched.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "sys/types.h"
#include "myFunc.h"
#define STACK_SIZE 1024*1024  // 1MB

int main(int argc, char* const argv[]) {
    printf("Number of Argument Received in [%s] program: [%d]\n", argv[0], argc-1);
    pid_t *pids = (pid_t*)malloc(sizeof(pid_t) * (argc-1));
    void **stacks = (void**)malloc(sizeof(void*) * (argc-1));
    for(int i=0;i<argc-1;i++)
        stacks[i] = (void*)malloc(sizeof(void)*STACK_SIZE);

    for(int i=0; i<argc-1; i++) {
        pids[i] = clone(tableCalc, stacks[i]+STACK_SIZE, SIGCHLD, argv[i+1]);
    } for(int i=0;i<argc-1;i++)
        waitpid(pids[i], NULL, 0);

    printf("Code Execution Completed!!\n");
    return EXIT_SUCCESS;
}