#include "stdio.h"
#include "sys/types.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"

int main(int argc, char* const argv[]) {
    FILE *f1;

    int i, x = 0;
    printf("PID: [%d]\n", getpid()); // uses unistd.h and sys/types.h

    for (i = 0; i < 500; i++) {
        // reads current balance from text file
        f1 = fopen("01-acc_bal.txt", "r");
        fscanf(f1, "bal: %d", &x);
        fclose(f1);

        if(fork()) {
            x++;
            //  writes updated balance to text file
            f1 = fopen("01-acc_bal.txt", "w+");
            fprintf(f1, "bal: %d", x);
            fclose(f1);
        } else {
            //  writes updated balance to text file
            x+=2;
            f1 = fopen("01-acc_bal.txt", "w+");
            fprintf(f1, "bal: %d", x);
            fclose(f1);
            exit(EXIT_SUCCESS);
        }
    } for(int i=0;i<500;i++)
        wait(NULL);
    return EXIT_SUCCESS;
}
