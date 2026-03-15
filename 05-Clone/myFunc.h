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

typedef struct {
    int** mat;
    int* res;
    int idx;
    int* ans;
    int n;
} MatData;

void cofactor(int **mat, int **temp, int p, int q, int n) {
    int i = 0, j = 0;
    for (int row = 0; row < n; row++) 
        for (int col = 0; col < n; col++) 
            if (row != p && col != q) {
                temp[i][j++] = mat[row][col];
                if (j == n - 1) {
                    j = 0;
                    i++;
                }
            }
}

int determinant(int **mat, int n) {
    int det = 0; 
    if (n == 1)
        return mat[0][0];

    int **temp = (int**)malloc(sizeof(int*)*n);
    for(int i=0;i<n;i++)
        temp[i] = (int*)malloc(sizeof(int)*n); 
    int sign = 1; 
    for (int f = 0; f < n; f++) {
        cofactor(mat, temp, 0, f, n);
        det += sign * mat[0][f] * determinant(temp, n - 1);
        sign = -sign;
    }
    return det;
}

void input(int** mat, int* res, int n) {
    for(int i=0;i<n;i++) {
        printf("Enter Value of: \n");
        printf("[%d]: ", i+1);
        for(int j=0;j<n;j++) {
            printf("%cx%d", 65+j, j+1);
            j!=n-1? printf(" + ") : printf(" = ");
        } printf("a; \n");
        for(int j=0;j<=n;j++) {
            if(j<=n-1) 
                scanf("%d", &mat[i][j]);
            else    
                scanf("%d", &res[i]);
        } 
    }
}

void displayEqn(int** mat, int* res, int n) {
    printf("\n\nProvided Eqns: \n");
    for(int i=0;i<n;i++) {
        for(int j=0;j<n;j++) {
            if(!j && mat[i][j]<0) 
                printf("-");
            else if(j) {
                if(mat[i][j]>=0) 
                    printf(" + ");
                else 
                    printf(" - ");
            }
            printf("%dx%d", abs(mat[i][j]), j+1);
            if(j==n-1) 
                printf(" = ");
        } printf("%d \n", res[i]);
    }
}

void colneMat(int** mat, int** temp, int n) {
    for(int i=0;i<n;i++) 
        for(int j=0;j<n;j++) 
            temp[i][j] = mat[i][j];
    
}

void replace(int** mat, int* res, int n, int idx) {
    for(int i=0;i<n;i++) 
        mat[i][idx] = res[i];
}

void free2D(int** mat, int n) {
    for(int i=0;i<n;i++)
        free(mat[i]);
    free(mat);
}

int crammerFunc(void* args) {
    MatData* data = (MatData*)args;
    
    int** temp = (int**)malloc(sizeof(int*) * data->n);
    for(int i=0;i<data->n;i++) 
        temp[i] = (int*)malloc(sizeof(int) * data->n);

    colneMat(data->mat, temp, data->n);
    
    replace(temp, data->res, data->n, data->idx);
    
    int det = determinant(temp, data->n);
    
    data->ans[(data->idx)+1] = det; 
    free2D(temp, data->n);
    return EXIT_SUCCESS;
}