#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "myFunc.h"

int main(int argc, char* const argv[]) {
    int n;
    printf("Enter Number of Variables/Equations[n]: ");
    scanf("%d", &n);
    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * n);
    int* res = (int*)malloc(sizeof(int) * n);
    int **mat = (int**)malloc(sizeof(int*) * n);
    for(int i=0;i<n;i++)
        mat[i] = (int*)malloc(sizeof(int) * n);
    input(mat, res, n);
    displayEqn(mat, res, n);
    int det = determinant(mat, n);
    void* soln;
    for(int i=0;i<n;i++) {
        MatData* args = (MatData*)malloc(sizeof(MatData));
        args->idx = i;
        args->res = res;
        args->mat = mat;
        args->n = n;
        pthread_create((threads+i), NULL, crammerFunc, (void*)args);   
    } printf("\n\nSolns: \n");
    for(int i=0;i<n;i++) {
        pthread_join(threads[i], &soln);
        printf("x%d: [%.2f]\n", i+1,(float)(*(int*)soln)/(float)det);
    }
    return EXIT_SUCCESS;
}