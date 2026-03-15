#define _GNU_SOURCE
#include "sched.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "myFunc.h"
#define STACK_SIZE 1024*1024 // 1MB


int main(int argc, char* const argv[]) {
    int n;
    printf("Enter Number of Variables/Equations[n]: ");
    scanf("%d", &n);
    pid_t *pids = (pid_t*)malloc(sizeof(pid_t) * n);
    void** stacks = (void**)malloc(sizeof(void*) * n);
    for(int i=0;i<n;i++)
        stacks[i] = (void*)malloc(sizeof(void) * STACK_SIZE);
    int* res = (int*)malloc(sizeof(int) * n);
    int* ans = (int*)malloc(sizeof(int) * (n+1));
    int **mat = (int**)malloc(sizeof(int*) * n);
    for(int i=0;i<n;i++)
        mat[i] = (int*)malloc(sizeof(int) * n);
    input(mat, res, n);
    displayEqn(mat, res, n);
    for(int i=0;i<n;i++) {
        MatData* args = (MatData*)malloc(sizeof(MatData));
        args->idx = i;
        args->ans = ans;
        args->res = res;
        args->mat = mat;
        args->n = n;
        pids[i] = clone(crammerFunc, stacks[i] + STACK_SIZE, CLONE_VM | SIGCHLD, args);
    } ans[0] = determinant(mat, n);
    for(int i=0;i<n;i++)
        waitpid(pids[i], NULL, 0);
    for(int i=1;i<=n;i++) {
        printf("x%d = [%.2f]\n", i, (float)ans[i]/(float)ans[0]);
    }
    return EXIT_SUCCESS;
}