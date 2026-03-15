#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "unistd.h"

bool isPrime(int n) {
    if(n<=1) return false;
    if(n<=3) return true;
    for(int i=2;i*i<=n;i++)
        if(!(n%i))
            return false;
    return true;
}

typedef struct {
    int start;
    int end;
    int idx;
    int* res;
} ThreadArgs;

int primeCalculation(void* args) {
    ThreadArgs* arg = (ThreadArgs*)args;
    int cnt = 0;
    for(int i=arg->start;i<=arg->end;i++)
        if(isPrime(i))
            cnt++;

    arg->res[arg->idx] = cnt;

    return EXIT_SUCCESS;
}

int tableCalc(void* args) {
    int num = atoi(args);
    printf("Thie Process is Running under Child ID: [%d]\n", getpid());
    fflush(stdout);
    printf("[%d]: ", num);
    for(int i=1;i<=10;i++)
        printf("[%d * %d = %d] ", i, num, i*num);
    printf("\n");
    return EXIT_SUCCESS;

}