#include "stdio.h"      // printf, fopen, fclose, fgetc, fputc
#include "ctype.h"      // tolower and toupper
#include "sys/types.h"  // pid_t
#include "stdlib.h"     // EXIT_SUCCESS
#include "unistd.h"     // fork
#include "sys/wait.h"   // wait

int main(int argc, char *const argv[]) {
    FILE *f1, *f2;
    char c;
    pid_t pid;

    pid = fork();

    if (pid) { // Parent Process
        f1 = fopen("01-demo.txt", "r");
        f2 = fopen("01-demo-copy.txt", "w");
        while ((c = fgetc(f1)) != EOF) {
            c = tolower(c);
            printf("%c", c);
        } 
    } else {  // Child Process
        f1 = fopen("01-demo.txt", "r");
        f2 = fopen("01-demo-copy.txt", "w");
        while ((c = fgetc(f1)) != EOF) {
            c = toupper(c);
            fputc(c, f2);
        }
    }
    
    printf("\nDone");
    fclose(f1);
    fclose(f2);
    return EXIT_SUCCESS;
}