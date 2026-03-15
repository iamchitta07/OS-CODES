#define _GNU_SOURCE
#include "sched.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "myFunc.h"
#define STACK_SIZE 1024*1024 // 1 MB

int main(int argc, char* const argv[]) {
    int n, low, upp;
    printf("Enter Number of Threads[n]: ");
    scanf("%d", &n);
    printf("Enter lower limit[low]: ");
    scanf("%d", &low);
    printf("Enter upper limit[upper]: ");
    scanf("%d", &upp);
    
    int chunk = (upp-low+1)/n;

    int *res = (int*)malloc(sizeof(int) * n);
    pid_t *pids = (pid_t*)malloc(sizeof(pid_t) * n);
    void **stacks = (void**)malloc(sizeof(void*) * n);

    for(int i=0;i<n;i++) {
        stacks[i] = (void*)malloc(sizeof(void) * STACK_SIZE);
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->start = low + i*chunk;
        args->end = i==n-1? upp : low + (i+1)*chunk - 1;
        args->idx = i;
        args->res = res;
        
        pids[i] = clone(primeCalculation, stacks[i]+STACK_SIZE, CLONE_VM | SIGCHLD, args);

        if(pids[i] == -1) {
            perror("Clone Failed!!");
            exit(EXIT_FAILURE);
        }
    }
    for(int i=0;i<n;i++) 
        waitpid(pids[i], NULL, 0);

    int total = 0;
    for(int i=0;i<n;i++) {
        printf("Range: [%d, %d], Primes Found: [%d]\n", low+i*chunk, i==n-1 ? upp : low + (i+1)*chunk - 1, res[i]);
        total+=res[i];
    } printf("Total Prime Found: [%d]\n", total);

    for(int i=0;i<n;i++)
        free(stacks[i]);

    free(stacks);
    free(res);
    free(pids);

    return EXIT_SUCCESS;
}