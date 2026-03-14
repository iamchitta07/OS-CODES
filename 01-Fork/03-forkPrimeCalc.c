#include "myFunc.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"

int main(int argc, char* const argv[]) {
    int n, lt, rt;
    printf("Enter Number of Process[n]: ");
    scanf("%d", &n);
    printf("Enter Lower Limit[lt]: ");
    scanf("%d", &lt);
    printf("Enter Upeer Limit[rt]: ");
    scanf("%d", &rt);
    int chunk = (rt-lt)/n;
    printf("Chunk Size: [%d]\n", chunk);
    for(int i=0;i<n;i++) {
        if(!fork()) {
            int left = lt + i*chunk;
            int right = i==n-1 ? rt : lt + (i+1)*chunk - 1;
            int cnt = 0;
            for(int j=left;j<=right;j++) 
                if(isPrime(j))
                    cnt++;
    
            printf("Fork Id: [%d], PID: [%d], Searching Primes in [%d, %d], Found %d Prime Numbers\n", i+1, getpid(), left, right, cnt);
            exit(EXIT_SUCCESS);
        }
    }
    for(int i=0;i<n;i++)
        wait(NULL);
    return EXIT_SUCCESS;
}