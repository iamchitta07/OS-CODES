#define _GNU_SOURCE
#include "sched.h"
#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "sys/wait.h"
#include "sys/types.h"
#define STACK_SIZE 1024*1024 // 1MB

int childFunc(void *arg) {
    printf("Hello from child\n");
    return EXIT_SUCCESS;
}

int main(int argc, char* const argv[]) {

    char stack[STACK_SIZE];

    pid_t pid = clone(childFunc, stack + STACK_SIZE, CLONE_VM | CLONE_FS | CLONE_FILES | SIGCHLD, NULL);

    printf("Parent running\n");
    waitpid(pid, NULL, 0);

    return EXIT_SUCCESS;
}