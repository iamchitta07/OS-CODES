#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "unistd.h"
#include "pthread.h"

//============= Prime Calculation ===================//

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
} ThreadArgs;

void* primeCalculation(void* args) {
    ThreadArgs* arg = (ThreadArgs*)args;
    int *cnt = (int*)calloc(1, sizeof(int));
    for(int i=arg->start;i<=arg->end;i++)
        if(isPrime(i))
            (*cnt)++;

    pthread_exit(cnt);
}

//================= Cramer's Rule ===================//

typedef struct {
    int** mat;
    int* res;
    int idx;
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

void* crammerFunc(void* args) {
    MatData* data = (MatData*)args;
    
    int** temp = (int**)malloc(sizeof(int*) * data->n);
    for(int i=0;i<data->n;i++) 
        temp[i] = (int*)malloc(sizeof(int) * data->n);

    colneMat(data->mat, temp, data->n);
    
    replace(temp, data->res, data->n, data->idx);
    
    int *det = (int*)malloc(sizeof(int)); 
    *det = determinant(temp, data->n);
    
    free2D(temp, data->n);
    pthread_exit(det);
    free(det);
    return NULL;
}


//============= Matrix Multiplication ===============//
int generateRand() {
    srand(time(NULL));
    return rand();
}

typedef struct {
    int** mat1;
    int** mat2;
    int** res;
    int idx;
    bool* flag;
    int* row1;
    int* colRow;
    int* col2;
} MatrAttr;

void inputMat(int** mat, int row, int col) {
    // printf("Enter Elements of [%d X %d] Matrix!!\n", row, col);
    for(int i=0;i<row;i++) 
        for(int j=0;j<col;j++) 
            mat[i][j] = generateRand();
            // scanf("%d", &mat[i][j]);
        
}

int maxVal(int a, int b) {
    return a>=b ? a : b;
}

void outputMat(int** mat, int row, int col) {
    for(int i=0;i<row;i++) {
        for(int j=0;j<col;j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
}

void* matMul(void* args) {
    MatrAttr* data = (MatrAttr*)args;
    if(*(data->flag)) {
        for(int i=0;i<(*(data->col2));i++) {
            int sum = 0;
            int idx = data->idx;

            for(int j=0;j<(*(data->colRow));j++) 
                sum+=(data->mat1[idx][j]*data->mat2[j][i]);
            
            data->res[idx][i] = sum;
        }
    } else {
        for(int i=0;i<(*(data->row1));i++) {
            int sum = 0;
            int idx = data->idx;

            for(int j=0;j<(*(data->colRow));j++) 
                sum+=(data->mat1[i][j]*data->mat2[j][idx]);
            
            data->res[i][idx] = sum;
        }
    }
    return NULL;
}