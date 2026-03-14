#include "stdio.h"
#include "stdlib.h"
#include "signal.h"

// Custom Handler for SIGINT(Ctrl+C)
void fn1(int sig) {
    static int n;
    ++n;
    printf("\tReceived SIGINT[%d] Signal for [%d] Times.\n", sig, n);
    fflush(stdout);
    if(n==5) {
        printf("\tNext time I can't withstand the blow.....\n"); 
        fflush(stdout);
    	signal(SIGINT, SIG_DFL); // Set to Default Behaviour
    }
}

// Custom Handler for SIGINT(Ctrl+C)
void fn2(int sig) {
    static int n;
    ++n;
    printf("\tReceived SIGTSTP[%d] Signal for [%d] Times.\n", sig, n);
    fflush(stdout);
    if(n==5) {
        printf("\tNext time I can't withstand the blow.....\n"); 
        fflush(stdout);
    	signal(SIGTSTP, SIG_DFL); // Set to Default Behaviour
    }
}


int main(int argc, char *const argv[]) {
    signal(SIGINT, fn1);    // Overwritting the Signal Handler SIGINT(Ctrl+C)
    signal(SIGTSTP, fn2);    // Overwritting the Signal Handler SIGTSTP(Ctrl+Z)
    while(1);
    return EXIT_SUCCESS;
}