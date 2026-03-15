#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "myFunc.h"

int main(int argc, char* const argv[]) {
    int n, low, high;
    void* res;
    printf("Enter Number of Threads[n]: ");
    scanf("%d", &n);
    printf("Enter Lower Limit[low]: ");
    scanf("%d", &low);
    printf("Enter Upper Limit[upp]: ");
    scanf("%d", &high);
    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * n); 

    int chunk = (high-low+1)/n;

    for(int i=0;i<n;i++) {
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->start = low + i*chunk;
        args->end = i==n-1 ? high : args->start + chunk - 1;
        args->idx = 0; 
        pthread_create((threads+i), NULL, primeCalculation, (void*)args);
    }

    int total = 0;

    for(int i=0;i<n;i++) {
        pthread_join(threads[i], &res);
        printf("Number of Primes in Range [%d, %d]: [%d]\n", low + i*chunk, i==n-1? high : low + (i+1)*chunk -1, *(int*)res);
        total += (*(int*)res);
    } printf("Total Number of Threads: [%d]\n", total);
    free(threads);
    free(res);
    return EXIT_SUCCESS;
}