#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "myFunc.h"


int main(int argc, char* const argv[]) {
    struct timespec  start, end;
    int row1, colRow, col2;
    printf("Enter Row of First Matrix: ");
    scanf("%d", &row1);
    printf("Enter Col of First and Row of Second Matrix: ");
    scanf("%d", &colRow);
    printf("Enter Col of Second Matrix: ");
    scanf("%d", &col2);

    int** mat1 = (int**)malloc(sizeof(int*) * row1);
    int** mat2 = (int**)malloc(sizeof(int*) * colRow);
    int** res = (int**)malloc(sizeof(int*) * row1);

    for(int i=0;i<row1;i++)
        mat1[i] = (int*)malloc(sizeof(int) * colRow);
    for(int i=0;i<colRow;i++)
        mat2[i] = (int*)malloc(sizeof(int) * col2);
    for(int i=0;i<row1;i++)
        res[i] = (int*)malloc(sizeof(int) * col2);

    inputMat(mat1, row1, colRow);
    inputMat(mat2, colRow, col2);

    clock_gettime(1, &start);
    for(int i=0;i<row1;i++) {
        for(int j=0;j<col2;j++) {
            res[i][j] = 0;
            for(int k=0;k<colRow;k++) 
                res[i][j]+=(mat1[i][k]*mat2[k][j]);
            
        }
    }
    clock_gettime(1, &end);

    double totalSeconds = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Execution Time: [%.4f]s\n", totalSeconds);
    return EXIT_SUCCESS;
}